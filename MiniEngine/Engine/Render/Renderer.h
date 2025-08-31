﻿// ReSharper disable CppClangTidyBugproneBranchClone
#pragma once
#include "DynamicRHI.h"
#include "Material.h"
#include "RenderItem.h"
#include "RHIDefination.h"
#include "RHIConfiguration.h"
#include "RHIPipelineStateInializer.h"
#include "Shader.h"
#include "Engine/pch.h"

struct PassConstants
{
    LightConstant mLightConstant;
    FogConstant mFogConstant;
    Vector4 mTime; // deltaTime, time, sin(time), cos(time)

    // these should be in per-camera constants
    Vector4 mScreenParams; // width, height, preserved, preserved
};

class Renderer : public Singleton<Renderer>
{
    friend class Singleton<Renderer>;
public:
    using RenderPass = std::function<void()>;
    // multi-thread supported
    static std::unique_ptr<Material> createMaterial(RHIShader* pShader);

    // single thread only to guarantee the id generation.
    std::unique_ptr<MaterialInstance> createMaterialInstance(const Material& material);

    void initialize();
    void initialize(const RendererConfiguration& configuration);
    template<typename TRHI, typename = std::enable_if_t<std::is_base_of<RHI, TRHI>::value>>
    TRHI* getRHI() const;
    const RendererConfiguration& getConfiguration() const;

    // support multi-thread
    // this func won't check repeated compiling, it may cause redundant memory usage.
    std::unique_ptr<RHIShader> compileShader(const Blob& blob, ShaderType shaderTypes, const std::string* path = nullptr) const;
    // support multi-thread
    // this func won't check repeated compiling, it may cause redundant memory usage.
    std::unique_ptr<RHIShader> compileShader(const Blob& blob, ShaderType shaderTypes, const std::wstring* path = nullptr) const;
    // support multi-thread
    ShaderRef compileAndRegisterShader(const std::string& shaderName, const Blob& blob, ShaderType shaderTypes, const std::string* path = nullptr);
    ShaderRef compileAndRegisterShader(const std::wstring& shaderName, const Blob& blob, ShaderType shaderTypes, const std::wstring* path = nullptr);
    ShaderRef registerShader(std::unique_ptr<RHIShader>&& shader);
    // multi-thread supported
    void releaseShader(ShaderRef& shaderRef);

    void appendRenderLists(std::unique_ptr<RenderList[]> renderLists, uint32_t numRenderLists)
    {
        mRenderLists.reserve(mRenderLists.size() + numRenderLists);
        mRenderLists.insert(mRenderLists.end(), renderLists.get(), renderLists.get() + numRenderLists);
    }
    bool isRenderListEmpey() const
    {
        return mRenderLists.empty();
    }
    void setLightConstants(const LightConstant& lightConstants) const;
    void setTime(float deltaTime, float time) const;
    void setFogConstants(const FogConstant& fogConstants) const;

    VertexBufferRef allocVertexBuffer(uint32_t numVertices, uint32_t vertexSize);
    IndexBufferRef allocIndexBuffer(uint32_t numIndices, Format indexFormat);
    TextureRef allocTexture2D(Format format, uint32_t width, uint32_t height, uint8_t mipLevels);
    void updateVertexBuffer(const void* pData, uint64_t bufferSize, VertexBufferRef vertexBufferGPU, bool blockRendering = true);
    void updateIndexBuffer(const void* pData, uint64_t bufferSize, IndexBufferRef indexBufferGPU, bool blockRendering = true);
    void updateTexture(const void* pData, TextureRef textureGPU, uint8_t mipmap, bool blockRendering = true);

    void EnqueueRenderPass(RenderPass renderPass);

    //void releaseMaterialInstance(uint64_t instanceId);
    void render();

    Renderer();

    NON_COPYABLE(Renderer);
    NON_MOVEABLE(Renderer);

private:
    struct RenderContext
    {
        std::unique_ptr<RHIGraphicsContext> mGraphicContext;
        std::unique_ptr<RHIFence> mFenceGPU;
        std::vector<RHIConstantBuffer*> mReleasingCBuffers;
        uint64_t mFenceCPU;
    };
    uint32_t allocGPUResource(RHIObject* pObject);
    void createBuiltinResources();
    void beginFrame(RenderContext* pRenderContext);
    // sphere mode only now
    void skyboxPass(RHIGraphicsContext* pRenderContext, const RHIShader& skyboxShader, SkyboxType type, const CameraConstants& cameraConstants);
    void depthPrePass(RHIGraphicsContext* pRenderContext, const std::vector<RenderItem>& renderItems, const CameraConstants& cameraConstants);
    void opaquePass(RHIGraphicsContext* pRenderContext, const std::vector<RenderItem>& renderItems, const CameraConstants& cameraConstants);
    void postRender();

    static IndexBufferRef sQuadMeshIndexBuffer;
    static ShaderRef sPreDepthShader;

    RendererConfiguration mConfiguration;
    RHI* mRenderHardwareInterface;

    std::unique_ptr<RHISwapChain> mSwapChain;
    RHIDepthStencil* mDepthStencilBuffer;
    std::unique_ptr<RenderContext[]> mRenderContexts;
    uint8_t mNumRenderContexts;
    uint8_t mCurrentRenderContextIndex;

    std::unique_ptr<RHICopyContext> mCopyContext;
    std::unique_ptr<RHIFence> mCopyFenceGPU;
    uint64_t mCopyFenceCPU;

    enum PsoPresets : uint16_t
    {
        PSO_PRE_DEPTH,
        PSO_SKY_BOX,
        PSO_SHADOW,
        PSO_OPAQUE,
        PSO_TRANSPARENT,
        NUM_PRESETS
    };
    std::vector<PipelineInitializer> mPipeStateInitializers;

    std::vector<RenderList> mRenderLists;
    std::vector<RenderPass> mCustomRenderPasses;

    // ----------------Pass Constants----------------
    std::unique_ptr<PassConstants> mPassConstants;
    // ----------------------------------------------

    // -------------GPU Resource Manager-------------
    std::deque<std::unique_ptr<RHIObject>> mGPUResources;
    std::stack<uint64_t>    mAvailableGPUResourceIds;
    // ----------------------------------------------

    // ----------------Shader Library----------------
    std::mutex mShaderRegisterMutex;
    std::vector<std::unique_ptr<RHIShader>> mShaders;
    std::stack<uint64_t> mAvailableShaderId;
    // ----------------------------------------------


    // ---------------Material Engine----------------
    /*std::vector<MaterialInstance> mMaterialInstances;
    std::stack<uint64_t> mAvailableMaterialInstanceId;*/
    // ----------------------------------------------
};