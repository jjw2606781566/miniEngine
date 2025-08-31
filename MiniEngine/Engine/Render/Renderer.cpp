#include "Renderer.h"

#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
#include "Engine/Dependencies/imGui/imgui_impl_dx12.h"
#include "PC/Private/D3D12GraphicsContext.h"
#include "PC/D3D12RHI.h"
#endif

std::unique_ptr<Material> Renderer::createMaterial(RHIShader* pShader)
{
	ASSERT(pShader, TEXT("mShader must not be null."));
	uint32_t numCBuffers = 0;
	uint32_t numTextures = 0;
	uint32_t numSamplers = 0;
	const std::vector<ShaderProp>& properties = pShader->GetShaderProperties();
	for (auto& shaderProp : properties)
	{
		switch (shaderProp.mType)
		{
		case ShaderPropType::CBUFFER:
			numCBuffers += shaderProp.mRegister >= 2;	// TODO: magic number.
			break;
		case ShaderPropType::TEXTURE:
			numTextures++;
			break;
		case ShaderPropType::SAMPLER:
			numSamplers++;
			break;
		}
	}
	Material* material = new Material();
	material->mNumConstants = numCBuffers;
	material->mNumTextures = numTextures;
	material->mNumSamplers = numSamplers;
	material->mConstants.reset(numCBuffers ?  new ConstantProperty[numCBuffers] : nullptr);
	material->mTextures.reset(numTextures ? new TextureProperty[numTextures] : nullptr);
	material->mSamplers.reset(numSamplers ? new SamplerProperty[numSamplers] : nullptr);
	material->mShader = pShader;
	numCBuffers = 0;
	numTextures = 0;
	numSamplers = 0;
	for (uint32_t i = 0; i < properties.size(); ++i)
	{
		const ShaderProp& prop = properties[i];
#ifdef UNICODE
		std::wstring&& propName = ::AsciiToUtf8(prop.mName);
#else
		std::string& propName = prop.mName;
#endif
		if (prop.mType == ShaderPropType::CBUFFER && prop.mRegister >= 2)
		{
			material->mConstants[numCBuffers] = ConstantProperty{ propName, prop.mRegister, prop.mInfo.mCBufferSize };
			numCBuffers++;
		}
		else if (prop.mType == ShaderPropType::TEXTURE)
		{
			material->mTextures[numTextures] = TextureProperty{ propName, prop.mRegister, prop.mInfo.mTextureDimension };
			numTextures++;
		}
		else if (prop.mType == ShaderPropType::SAMPLER)
		{
			//material->mSamplers[numSamplers] = { propName };
			numSamplers++;
		}
	}
	return std::unique_ptr<Material>(material);

}

std::unique_ptr<MaterialInstance> Renderer::createMaterialInstance(const Material& material)
{
	static uint64_t id = 0;
	MaterialInstance* materialInstance = new MaterialInstance{};
	materialInstance->SetMaterialInstanceId(id++);
	materialInstance->InstantiateFrom(&material);
	return std::unique_ptr<MaterialInstance>(materialInstance);
}

void Renderer::initialize()
{
	initialize(RendererConfiguration::Default());
}

void Renderer::initialize(const RendererConfiguration& configuration)
{
	mPassConstants.reset(new PassConstants{});

	// initialize render hardware interface(rhi).
#ifdef WIN32
	mRenderHardwareInterface = new D3D12RHI();
#else
	mRenderHardwareInterface = new PlayStationRHI();
#endif
	RHIConfiguration rhiConfiguration = RHIConfiguration::Default();
	mRenderHardwareInterface->Initialize();

	RHISwapChainDesc swapChainDesc;
	swapChainDesc.mFormat = configuration.mBackBufferFormat;
	swapChainDesc.mWidth = 0;
	swapChainDesc.mHeight = 0;
	swapChainDesc.mNumBackBuffers = configuration.mNumBackBuffers;
	swapChainDesc.mIsFullScreen = false;
	swapChainDesc.mMSAA = 1;
	swapChainDesc.mTargetFramesPerSec = 0;   // use zero to force the native display's refresh rate. 
	mSwapChain = mRenderHardwareInterface->RHICreateSwapChain(swapChainDesc);

	// Create depth-stencil buffer
	RHITextureDesc depthTextureDesc = mSwapChain->GetBackBufferDesc();
	depthTextureDesc.mFormat = configuration.mDepthStencilFormat;
	mDepthStencilBuffer = mRenderHardwareInterface->RHIAllocDepthStencil(depthTextureDesc).release();
	mGPUResources.emplace_back(mDepthStencilBuffer);

	// Create render contexts, number of render contexts must not be larger than back buffer count.
	mNumRenderContexts = configuration.mNumBackBuffers;
	mCurrentRenderContextIndex = 0;
	RHIGraphicsContext* pRenderContext;
	mRenderContexts.reset(new RenderContext[mNumRenderContexts]);
	for (uint8_t i = 0; i < mNumRenderContexts; ++i)
	{
		mRenderHardwareInterface->RHICreateGraphicsContext(&pRenderContext);
		mRenderContexts[i].mGraphicContext = std::unique_ptr<RHIGraphicsContext>(pRenderContext);
		mRenderContexts[i].mFenceGPU = mRenderHardwareInterface->RHICreateFence();
		mRenderContexts[i].mFenceCPU = 0;
	}

	RHICopyContext* pCopyContext;
	mRenderHardwareInterface->RHICreateCopyContext(&pCopyContext);
	mCopyContext.reset(pCopyContext);
	mCopyFenceGPU = mRenderHardwareInterface->RHICreateFence();
	mCopyFenceCPU = 0;

	createBuiltinResources();

	// prepare pipeline states
	mPipeStateInitializers.resize(NUM_PRESETS);
	PipelineInitializer& preDepth = mPipeStateInitializers[PSO_PRE_DEPTH];
	preDepth = PipelineInitializer::Default();
	ASSERT(sPreDepthShader.mObject, TEXT("missing mShader : PreDepth"));
	preDepth.SetShader(sPreDepthShader.mObject);
	preDepth.SetBlend(false, false, BlendDesc::Disabled());
	preDepth.SetDepthBias(DepthBiasSet::NORMAL);

	PipelineInitializer& skybox = mPipeStateInitializers[PSO_SKY_BOX];
	skybox = PipelineInitializer::Default();
	skybox.SetBlend(false, false, BlendDesc::Disabled());
	DepthTestDesc&& depthTest = DepthTestDesc::Default();
	depthTest.mDepthWriteMask = 0;
	skybox.SetDepthTest(depthTest);
	skybox.SetCullMode(CullMode::FRONT);
	skybox.SetStencilTest(StencilTestDesc::Disabled());

	PipelineInitializer& shadow = mPipeStateInitializers[PSO_SHADOW];
	shadow = PipelineInitializer::Default();

	PipelineInitializer& opaque = mPipeStateInitializers[PSO_OPAQUE];
	opaque = PipelineInitializer::Default();
	opaque.SetBlend(false, false, BlendDesc::Disabled());

	PipelineInitializer& transparent = mPipeStateInitializers[PSO_TRANSPARENT];
	transparent = PipelineInitializer::Default();

	uint16_t quadIndices[6] = { 0, 1, 2, 0, 2, 3 };
	sQuadMeshIndexBuffer = allocIndexBuffer(6, Format::R16_UINT);
	updateIndexBuffer(quadIndices, sizeof(uint16_t) * 6, sQuadMeshIndexBuffer, true);

	mConfiguration = configuration;
}

const RendererConfiguration& Renderer::getConfiguration() const 
{
	return mConfiguration;
}

VertexBufferRef Renderer::allocVertexBuffer(uint32_t numVertices, uint32_t vertexSize)
{
	RHIVertexBuffer* pVertexBuffer = mRenderHardwareInterface->RHIAllocVertexBuffer(vertexSize, numVertices).release();
	return { allocGPUResource(pVertexBuffer), pVertexBuffer };
}

IndexBufferRef Renderer::allocIndexBuffer(uint32_t numIndices, Format indexFormat)
{
	RHIIndexBuffer* pIndexBuffer = mRenderHardwareInterface->RHIAllocIndexBuffer(numIndices, indexFormat).release();
	return { allocGPUResource(pIndexBuffer), pIndexBuffer };
}

TextureRef Renderer::allocTexture2D(Format format, uint32_t width, uint32_t height, uint8_t mipLevels)
{
	RHINativeTexture* pTexture = mRenderHardwareInterface->RHIAllocTexture({ format, TextureDimension::TEXTURE2D, width, height, 1, mipLevels, 1, 0 }).release();
	return { allocGPUResource(pTexture), pTexture };
}

void Renderer::updateVertexBuffer(const void* pData, uint64_t bufferSize, VertexBufferRef vertexBufferGPU, bool blockRendering)
{
	uint64_t size = std::min<uint64_t>(vertexBufferGPU->GetBuffer()->BufferSize(), bufferSize);
	auto&& stagingBuffer = mRenderHardwareInterface->RHIAllocStagingBuffer(size);
	mRenderHardwareInterface->RHIUpdateStagingBuffer(stagingBuffer.get(), pData, 0, size);
	mCopyFenceGPU->Wait(mCopyFenceCPU);
	mRenderHardwareInterface->RHIResetCopyContext(mCopyContext.get());
	mCopyContext->UpdateBuffer(vertexBufferGPU.mObject, stagingBuffer.get(), size, 0, 0);
	mRenderHardwareInterface->RHISubmitCopyCommands(mCopyContext.get());
	mRenderHardwareInterface->RHISyncCopyContext(mCopyFenceGPU.get(), ++mCopyFenceCPU);

	if (!blockRendering) return;;
	mRenderContexts[mCurrentRenderContextIndex].mGraphicContext->InsertFence(mCopyFenceGPU.get(), mCopyFenceCPU);
}

void Renderer::updateIndexBuffer(const void* pData, uint64_t bufferSize, IndexBufferRef indexBufferGPU, bool blockRendering)
{
	uint64_t size = std::min<uint64_t>(indexBufferGPU->GetBuffer()->BufferSize(), bufferSize);
	auto&& stagingBuffer = mRenderHardwareInterface->RHIAllocStagingBuffer(size);
	mRenderHardwareInterface->RHIUpdateStagingBuffer(stagingBuffer.get(), pData, 0, size);
	mCopyFenceGPU->Wait(mCopyFenceCPU);
	mRenderHardwareInterface->RHIResetCopyContext(mCopyContext.get());
	mCopyContext->UpdateBuffer(indexBufferGPU.mObject, stagingBuffer.get(), size, 0, 0);
	mRenderHardwareInterface->RHISubmitCopyCommands(mCopyContext.get());
	mRenderHardwareInterface->RHISyncCopyContext(mCopyFenceGPU.get(), ++mCopyFenceCPU);

	if (!blockRendering) return;;
	mRenderContexts[mCurrentRenderContextIndex].mGraphicContext->InsertFence(mCopyFenceGPU.get(), mCopyFenceCPU);
}

void Renderer::updateTexture(const void* pData, TextureRef textureGPU, uint8_t mipmap, bool blockRendering)
{
	const RHITextureDesc& desc = textureGPU->GetDesc();
	// TODO: support update multi-mips
	std::unique_ptr<RHIStagingBuffer> pStagingBuffer = mRenderHardwareInterface->RHIAllocStagingTexture(desc, mipmap);
	mRenderHardwareInterface->RHIUpdateStagingTexture(pStagingBuffer.get(), desc, pData, mipmap);
	mCopyFenceGPU->Wait(mCopyFenceCPU);
	mRenderHardwareInterface->RHIResetCopyContext(mCopyContext.get());
	mCopyContext->UpdateTexture(textureGPU.mObject, pStagingBuffer.get(), 0);
	mRenderHardwareInterface->RHISubmitCopyCommands(mCopyContext.get());
	mRenderHardwareInterface->RHISyncCopyContext(mCopyFenceGPU.get(), ++mCopyFenceCPU);

	if (!blockRendering) return;;
	mRenderContexts[mCurrentRenderContextIndex].mGraphicContext->InsertFence(mCopyFenceGPU.get(), mCopyFenceCPU);
}

void Renderer::EnqueueRenderPass(RenderPass renderPass)
{
	mCustomRenderPasses.push_back(renderPass);
}

std::unique_ptr<RHIShader> Renderer::compileShader(const Blob& blob, ShaderType shaderTypes,
	const std::string* path) const
{
	return mRenderHardwareInterface->RHICompileShader(blob, shaderTypes, path);
}


std::unique_ptr<RHIShader> Renderer::compileShader(const Blob& blob, ShaderType shaderTypes, const std::wstring* path) const
{
	if (path)
	{
		std::string&& pathStr = ::Utf8ToAscii(*path); 
		return mRenderHardwareInterface->RHICompileShader(blob, shaderTypes, &pathStr);
	}
	return mRenderHardwareInterface->RHICompileShader(blob, shaderTypes);
}

ShaderRef Renderer::compileAndRegisterShader(const std::string& shaderName, const Blob& blob, ShaderType shaderTypes,
	const std::string* path)
{
	return registerShader(compileShader(blob, shaderTypes, path));
}

ShaderRef Renderer::compileAndRegisterShader(const std::wstring& shaderName, const Blob& blob, ShaderType shaderTypes,
                                              const std::wstring* path)
{
	return registerShader(compileShader(blob, shaderTypes, path));
}

ShaderRef Renderer::registerShader(std::unique_ptr<RHIShader>&& shader)
{
	ShaderRef shaderRef{0, shader.release()};
	mShaderRegisterMutex.lock();
	if (!mAvailableShaderId.empty())
	{
		shaderRef.mIndex = mAvailableShaderId.top();
		mShaders[mAvailableShaderId.top()].reset(shaderRef.mObject);
		mAvailableShaderId.pop();
	}
	else
	{
		shaderRef.mIndex = mShaders.size();
		mShaders.emplace_back(shaderRef.mObject);
	}
	mShaderRegisterMutex.unlock();
	return shaderRef;
}

void Renderer::releaseShader(ShaderRef& shaderRef)
{
	if (shaderRef.mObject || shaderRef.mIndex == MAXUINT64)
	{
		WARN("invalid shader reference.");;
		return;
	}
	auto& target = mShaders[shaderRef.mIndex];
	if (target) 
	{
		mShaderRegisterMutex.lock();
		mAvailableShaderId.push(shaderRef.mIndex);
		target.reset();
		mShaderRegisterMutex.unlock();
	}
	shaderRef.mObject = nullptr;
	shaderRef.mIndex = MAXUINT64;
}



inline void Renderer::setLightConstants(const LightConstant& lightConstants) const
{
	mPassConstants->mLightConstant = lightConstants;
}

inline void Renderer::setTime(float deltaTime, float time) const
{
	if (mPassConstants->mTime.v.y == time) return;
	mPassConstants->mTime = { deltaTime, time, std::sin(time), std::cos(time) };
}

inline void Renderer::setFogConstants(const FogConstant& fogConstants) const
{
	mPassConstants->mFogConstant = fogConstants;
}

void Renderer::render()
{
	RenderContext& renderContext = mRenderContexts[mCurrentRenderContextIndex];
	RHIGraphicsContext* graphicsContext = renderContext.mGraphicContext.get();
	renderContext.mFenceGPU->Wait(renderContext.mFenceCPU);
	mRenderHardwareInterface->RHIResetGraphicsContext(graphicsContext);
	mSwapChain->BeginFrame(graphicsContext);

	for (auto& renderList : mRenderLists)
	{
		// TODO: support specify the render target and depth stencil.
		RHIRenderTarget* pRenderTarget = mSwapChain->GetCurrentColorTexture();
		RHIDepthStencil* pDepthStencil = mDepthStencilBuffer;
		RHITextureDesc rtDesc = pRenderTarget->GetTextureDesc();
		Viewport viewports[] = { Viewport{static_cast<float>(rtDesc.mWidth), static_cast<float>(rtDesc.mHeight), 0, 1} };
		Rect scissorRects[] = { Rect{0, 0, static_cast<int32_t>(rtDesc.mWidth), static_cast<int32_t>(rtDesc.mHeight)} };
		// TODO: Calculations like this can move to pre-render phase.

		mPipeStateInitializers[PSO_SKY_BOX].SetFrameBuffers(pRenderTarget->GetFormat(), pDepthStencil->GetFormat());
		mPipeStateInitializers[PSO_PRE_DEPTH].SetFrameBuffers(Format::UNKNOWN, pDepthStencil->GetFormat());
		mPipeStateInitializers[PSO_OPAQUE].SetFrameBuffers(pRenderTarget->GetFormat(), pDepthStencil->GetFormat());

		mPassConstants->mScreenParams = { viewports[0].mWidth, viewports[0].mHeight, 1, 1 };
		graphicsContext->SetViewPorts(viewports, 1);
		graphicsContext->SetScissorRect(scissorRects, 1);

		// graphicsContext->SetRenderTargetsAndDepthStencil(nullptr, 0, pDepthStencil);
		graphicsContext->SetRenderTargetsAndDepthStencil(&pRenderTarget, 1, pDepthStencil);
		if (renderList.mClearRenderTarget)
		{
			graphicsContext->ClearDepthStencil(pDepthStencil, true, true, 1, 0, nullptr, 0);
			graphicsContext->ClearRenderTarget(pRenderTarget, renderList.mBackGroundColor, scissorRects, 1);
		}

		// depthPrePass(graphicsContext, renderList.mOpaqueList, renderList.mCameraConstants);

		// graphicsContext->SetRenderTargetsAndDepthStencil(&pRenderTarget, 1, pDepthStencil);
		if (renderList.mSkyboxShader)
		{
			skyboxPass(graphicsContext, *renderList.mSkyboxShader, renderList.mSkyBoxType, renderList.mCameraConstants);
		}
		opaquePass(graphicsContext, renderList.mOpaqueList, renderList.mCameraConstants);

		// AHCommandList commandList{graphicsContext};
		// commandList.SetCameraColorTarget(pRenderTarget);
		// commandList.SetCameraDepthStencil(pDepthStencil);
		// commandList.SetViewports(viewports, 1);
		// commandList.SetScissorRects(scissorRects, 1);
		//for (const RenderPass& pass : mCustomRenderPasses)
		//{
		//	pass(commandList);
		//}
		//mRenderHardwareInterface->RHISubmitRenderCommands(graphicsContext);

		//mRenderHardwareInterface->RHIResetGraphicsContext(graphicsContext);
		//graphicsContext->SetViewPorts(viewports, 1);
		//graphicsContext->SetScissorRect(scissorRects, 1);


	}
#ifdef WIN32
	D3D12GraphicsContext* pGConstext = static_cast<D3D12GraphicsContext*>(graphicsContext); // static_cast<D3D12GraphicsContext*>(commandList.GetRHIGraphicsContext());
	ID3D12GraphicsCommandList* pCommandList = pGConstext->GetD3D12CommandList();
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pCommandList);
#endif
	mSwapChain->EndFrame(graphicsContext);
	mRenderHardwareInterface->RHISubmitRenderCommands(graphicsContext);
	mRenderHardwareInterface->RHISyncGraphicContext(renderContext.mFenceGPU.get(), ++renderContext.mFenceCPU);
	mSwapChain->Present();
	mCurrentRenderContextIndex = (mCurrentRenderContextIndex + 1) % mNumRenderContexts;

	mRenderLists.clear();
	mCustomRenderPasses.clear();
}

Renderer::Renderer() = default;

uint32_t Renderer::allocGPUResource(RHIObject* pObject)
{
	if (mAvailableGPUResourceIds.empty())
	{
		mGPUResources.emplace_back(pObject);
		return mGPUResources.size() - 1;
	}
	uint32_t index = static_cast<uint32_t>(mAvailableGPUResourceIds.top());
	mAvailableGPUResourceIds.pop();
	mGPUResources[index] = std::unique_ptr<RHIObject>(pObject);
	return index;
}

void Renderer::createBuiltinResources()
{
	const char* builtinShaderSources[] = {
		"cbuffer ObjectConstants : register(b1) { float4x4 m_model; float4x4 m_model_i; float4x4 m_view; float4x4 m_view_i; float4x4 m_projection; float4x4 m_projection_i; }; struct SimpleVertexInput{float3 position : POSITION;float3 normal : NORMAL;float2 uv : TEXCOORD;};struct FragInput{float4 position : SV_POSITION;};FragInput VsMain(SimpleVertexInput input){FragInput o;float4 worldPosition = mul(m_model, float4(input.position, 1));o.position = mul(m_projection, mul(m_view, worldPosition));return o;}",
	};

	Blob shaderSource{builtinShaderSources[0], strlen(builtinShaderSources[0])};
	sPreDepthShader = compileAndRegisterShader(TEXT("PreDepth"), shaderSource, ShaderType::VERTEX);
}

void Renderer::beginFrame(RenderContext* pRenderContext)
{
	auto& renderContext = *pRenderContext;
	mRenderHardwareInterface->RHIReleaseConstantBuffers(renderContext.mReleasingCBuffers.data(), renderContext.mReleasingCBuffers.size());
	renderContext.mReleasingCBuffers.clear();
	for (const RenderList& renderList : mRenderLists)
	{
		for (const RenderItem& opaqueItem : renderList.mOpaqueList)
		{
			MaterialInstance& materialInstance = *opaqueItem.mMaterial;
			if (!materialInstance.mIsDirty) continue;
			const Material& material = *materialInstance.mMaterial;
			// if the constants of material instance is dirty, reallocate constant buffers for this material.
			for (int i = 0; i < material.mNumConstants; ++i)
			{
				ConstantBufferRef& cbufferRef = materialInstance.mConstantsGPU[i];
				mGPUResources[cbufferRef.mIndex].release();
				renderContext.mReleasingCBuffers.push_back(cbufferRef.mObject);
				cbufferRef.mObject = mRenderHardwareInterface->RHIAllocConstantBuffer(material.mConstants[i].mConstantSize).release();
				mGPUResources[cbufferRef.mIndex].reset(cbufferRef.mObject);
				const Blob& cbufferCache = materialInstance.GetConstantBuffer(i);
				mRenderHardwareInterface->RHIUpdateConstantBuffer(cbufferRef.mObject, cbufferCache.Binary(), 0, cbufferCache.Size());
			}
		}
	}
	mRenderHardwareInterface->RHIResetGraphicsContext(pRenderContext->mGraphicContext.get());
}

void Renderer::skyboxPass(RHIGraphicsContext* pRenderContext, const RHIShader& skyboxShader,
                          SkyboxType type, const CameraConstants& cameraConstants)
{
	PipelineInitializer& skyboxPSO = mPipeStateInitializers[PSO_SKY_BOX];
	skyboxPSO.SetShader(&skyboxShader);

	struct alignas(256) SkyBoxConstants
	{
		Matrix4x4 mVP;
		Matrix4x4 mVPI;
		Vector4 mScreenParams;
	};

	switch (type)
	{
	// only procedural skybox supported
	case SkyboxType::SKYBOX_PROCEDURAL:
		{
		skyboxPSO.SetCullMode(CullMode::BACK);

		pRenderContext->SetPipelineState(skyboxPSO);
		SkyBoxConstants skyboxConstants{ cameraConstants.mProjection * cameraConstants.mView ,
			cameraConstants.mViewInverse * cameraConstants.mProjectionInverse, mPassConstants->mScreenParams };

		std::unique_ptr<RHIConstantBuffer> cbuffer = pRenderContext->AllocConstantBuffer(sizeof(LightConstant));
		pRenderContext->BeginBinding();
		mRenderHardwareInterface->RHIUpdateConstantBuffer(cbuffer.get(), &mPassConstants->mLightConstant, 0, sizeof(LightConstant));
		pRenderContext->SetConstantBuffer(0, cbuffer.get());

		cbuffer = pRenderContext->AllocConstantBuffer(sizeof(SkyBoxConstants));
		mRenderHardwareInterface->RHIUpdateConstantBuffer(cbuffer.get(), &skyboxConstants, 0, sizeof(SkyBoxConstants));
		pRenderContext->SetConstantBuffer(1, cbuffer.get());
		pRenderContext->SetVertexBuffers(nullptr, 0);
		pRenderContext->SetIndexBuffer(sQuadMeshIndexBuffer.mObject);
		pRenderContext->EndBindings();

		pRenderContext->DrawIndexedInstanced(6, 0, 0, 1, 0);
		}
		break;
	default:
		THROW_EXCEPTION(TEXT("unsupported skybox type."));
		break;
	}
}

void Renderer::depthPrePass(RHIGraphicsContext* pRenderContext, const std::vector<RenderItem>& renderItems, const CameraConstants& cameraConstants)
{
	PipelineInitializer& preDepthPSO = mPipeStateInitializers[PSO_PRE_DEPTH];
	TransformConstants transform{Matrix4x4::Identity, Matrix4x4::Identity, cameraConstants };
	for (const auto& renderItem : renderItems)
	{
		const MaterialInstance& material = *renderItem.mMaterial;
		const DepthTestDesc& depthTest = renderItem.mMaterial->DepthTest();
		if (!depthTest.mEnableDepthTest) continue;

		preDepthPSO.SetCullMode(material.GetCullMode());
		preDepthPSO.SetDrawMode(renderItem.mMaterial->GetDrawMode());
		preDepthPSO.SetDepthTest(depthTest);
		preDepthPSO.SetStencilTest(material.StencilTest());
		pRenderContext->SetPipelineState(preDepthPSO);

		transform.mModel = renderItem.mModel;
		transform.mModelInverse = renderItem.mModelInverse;
		std::unique_ptr<RHIConstantBuffer> cbuffer = pRenderContext->AllocConstantBuffer(sizeof(TransformConstants));
		mRenderHardwareInterface->RHIUpdateConstantBuffer(cbuffer.get(), &transform, 0, sizeof(TransformConstants));

		pRenderContext->BeginBinding();
		pRenderContext->SetConstantBuffer(1, cbuffer.get());

		RHIVertexBuffer* vertexBuffers[] = { renderItem.mMeshData.mVertexBuffer.mObject };
		pRenderContext->SetVertexBuffers(vertexBuffers, 1);
		pRenderContext->SetIndexBuffer(renderItem.mMeshData.mIndexBuffer.mObject);
		pRenderContext->EndBindings();

		const SubMesh* subMeshes = renderItem.mMeshData.mSubMeshes.get();
		uint32_t numSubMeshes = renderItem.mMeshData.mSubMeshCount;
		for (uint32_t i = 0; i < numSubMeshes; ++i)
		{
			// TODO: support gpu instancing.
			pRenderContext->DrawIndexedInstanced(subMeshes[i].mIndexNum, subMeshes[i].mStartIndex, subMeshes[i].mBaseVertex, 1, 0);
		}
	}
}

void Renderer::opaquePass(RHIGraphicsContext* pRenderContext, const std::vector<RenderItem>& renderItems, const CameraConstants& cameraConstants)
{
	PipelineInitializer& opaquePSO = mPipeStateInitializers[PSO_OPAQUE];
	TransformConstants transform{ Matrix4x4::Identity, Matrix4x4::Identity, cameraConstants };
	for (const auto& renderItem : renderItems)
	{
		// set pipeline states
		const MaterialInstance& material = *renderItem.mMaterial;
		opaquePSO.SetCullMode(material.GetCullMode());
		opaquePSO.SetDrawMode(material.GetDrawMode());
		opaquePSO.SetDepthTest(material.DepthTest());
		opaquePSO.SetStencilTest(material.StencilTest());
		opaquePSO.SetBlend(false, false, material.BlendMode());
		opaquePSO.SetFrameBuffers(mSwapChain->GetBackBufferDesc().mFormat, mDepthStencilBuffer->GetFormat());	// TODO:
		opaquePSO.SetShader(renderItem.mMaterial->GetShader());
		pRenderContext->SetPipelineState(opaquePSO);

		const MaterialInstance& materialInstance = *renderItem.mMaterial;

		pRenderContext->BeginBinding();
		// update and bind constants(uniforms)
		transform.mModel = renderItem.mModel;
		transform.mModelInverse = renderItem.mModelInverse;
		RHIConstantBuffer* pTransformCBuffer = pRenderContext->AllocConstantBuffer(sizeof(TransformConstants)).release();
		uint8_t numConstants = materialInstance.NumConstantBuffers() + 1;	// material constants + 1 transform constants
		std::unique_ptr<RHIConstantBuffer*[]> cbuffers = std::make_unique<RHIConstantBuffer*[]>(numConstants);
		cbuffers[0] = pTransformCBuffer;
		// TODO: 
		mRenderHardwareInterface->RHIUpdateConstantBuffer(pTransformCBuffer, &transform, 0, sizeof(TransformConstants));
		for (uint8_t i = 1; i < numConstants; ++i)
		{
			const Blob& constant = materialInstance.GetConstantBuffer(i - 1);
			cbuffers[i] = pRenderContext->AllocConstantBuffer(constant.Size()).release();
			mRenderHardwareInterface->RHIUpdateConstantBuffer(cbuffers[i], constant.Binary(), 0, constant.Size());
		}
		pRenderContext->SetConstantBuffers(1, numConstants, cbuffers.get());
		for (uint8_t i = 0; i < numConstants; ++i)
		{
			delete cbuffers[i];
		}

		// bind textures/instance buffer.
		uint8_t numTextures = materialInstance.NumTextures();
		for (uint8_t i = 0; i < numTextures; ++i)
		{
			RHINativeTexture* pTexture = materialInstance.GetTexture(i).mObject;
			if (!pTexture) continue;
			pRenderContext->SetTexture(i, pTexture);
		}

		// bind vertex buffers and index buffer.
		RHIVertexBuffer* vertexBuffers[] = { renderItem.mMeshData.mVertexBuffer.mObject };
		pRenderContext->SetVertexBuffers(vertexBuffers, 1);
		pRenderContext->SetIndexBuffer(renderItem.mMeshData.mIndexBuffer.mObject);
		pRenderContext->EndBindings();

		// append draw call
		const SubMesh* subMeshes = renderItem.mMeshData.mSubMeshes.get();
		uint32_t numSubMeshes = renderItem.mMeshData.mSubMeshCount;
		auto temp = renderItem;
		for (uint32_t i = 0; i < numSubMeshes; ++i)
		{
			// TODO: support gpu instancing.
			pRenderContext->DrawIndexedInstanced(subMeshes[i].mIndexNum, subMeshes[i].mStartIndex, subMeshes[i].mBaseVertex, 1, 0);
		}
	}
}

void Renderer::postRender()
{
        
}

IndexBufferRef Renderer::sQuadMeshIndexBuffer{};
ShaderRef Renderer::sPreDepthShader{MAXUINT64, nullptr};

#if defined(WIN32)
template<>
D3D12RHI* Renderer::getRHI<D3D12RHI>() const
{
	return static_cast<D3D12RHI*>(mRenderHardwareInterface);
}
#elif defined(ORBIS)
template<>
PlayStationRHI* Renderer::getRHI<PlayStationRHI>() const
{
	return static_cast<PlayStationRHI*>(mRenderHardwareInterface);
}
#endif