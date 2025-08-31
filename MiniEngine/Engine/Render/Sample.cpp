#include "Engine/pch.h"
#include <Engine/common/Exception.h>

#include "Engine/render/Renderer.h"
#include "Engine/Window/WFrame.h"

namespace Temp
{
    static void LoadShaderFile(const String& path, char** data, uint64_t* size)
    {
        // 1. 打开文件（二进制模式）
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw Exception((TEXT("Failed to open file: ") + path).c_str());
        }

        // 2. 获取文件大小
        std::streampos fileSize = file.tellg();
        if (fileSize == -1) {
            throw Exception((TEXT("Failed to get file Size: ") + path).c_str());
        }
        *size = static_cast<uint64_t>(fileSize);

        // 3. 分配内存（调用方需负责释放！）
        *data = new char[*size];

        // 4. 读取文件内容
        file.seekg(0, std::ios::beg);
        if (!file.read(*data, *size)) {
            delete[] *data; // 读取失败时释放内存
            *data = nullptr;
            *size = 0;
            throw Exception((TEXT("Failed to read file: ") + path).c_str());
        }
    }
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#if defined(DEBUG) or defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    WFrame& frame = dynamic_cast<WFrame&>(*WFrame::CreateFrame(TEXT("Sample Frame"), 1280, 720));

    // 初始化渲染器
    Renderer& renderer = Renderer::GetInstance();
    renderer.initialize();

    // 加载着色器
    std::vector<String> paths{
        TEXT("Assets/Shaders/source/Opaque.hlsl"),
		TEXT("Assets/Shaders/source/skybox.hlsl")
    };
    std::vector<uint8_t> activeShaders{
        ShaderType::VERTEX | ShaderType::PIXEL,
        ShaderType::VERTEX | ShaderType::PIXEL
    };

    RHIShader** shaders = new RHIShader * [paths.size()];
    for (uint32_t i = 0; i < activeShaders.size(); i++)
    {
        String& path = paths[i];
        char* data;
        uint64_t size;
        Temp::LoadShaderFile(paths[i], &data, &size);
        Blob shaderSource{ data, size };
        String name = ::GetFileNameFromPath(path);
        std::unique_ptr<RHIShader> pShader = renderer.compileShader(shaderSource, static_cast<ShaderType>(activeShaders[i]), &path);
        shaders[i] = renderer.registerShader(std::move(pShader));
        delete[] data; // 释放内存
    }

    // 创建材质
    std::unique_ptr<Material> material = Renderer::createMaterial(shaders[0]);
	std::unique_ptr<MaterialInstance> materialInstance = renderer.createMaterialInstance(*material);

    // 准备CPU资源
	Mesh cubeMesh = MeshPrototype::CreateCubeMesh();
    const std::vector<float>& vertexBufferCPU = cubeMesh.getFilteredVertexBuffer();
	const std::vector<uint32_t>& indexBufferCPU = cubeMesh.indices();
    byte* cubeTexCPU = new byte[2 * 2 * 4]{255, 255, 255, 255, 255, 255, 255, 255 , 255, 255, 255, 255 , 255, 255, 255, 255};

    // 分配并上传GPU资源
    TextureRef cubeTexGPU = renderer.allocTexture2D(Format::R8G8B8A8_UNORM, 2, 2, 1);
    VertexBufferRef vertexBufferGPU = renderer.allocVertexBuffer(vertexBufferCPU.size() >> 3, sizeof(float[3 + 3 + 2]));
	IndexBufferRef indexBufferGPU = renderer.allocIndexBuffer(indexBufferCPU.size(), Format::R32_UINT);
    // update synchronously
    renderer.updateVertexBuffer(vertexBufferCPU.data(), vertexBufferCPU.size() * sizeof(float), vertexBufferGPU, true);
    renderer.updateIndexBuffer(indexBufferCPU.data(), indexBufferCPU.size() * sizeof(uint32_t), indexBufferGPU, true);
    renderer.updateTexture(cubeTexCPU, cubeTexGPU, 0, true);

    materialInstance->SetTexture(0, TextureDimension::TEXTURE2D, cubeTexGPU);

    LightConstant lightConstants{};
    lightConstants.mAmbientLight = { 0, 0, 0 };
    lightConstants.mAmbientIntensity = 1;
    lightConstants.mMainLightColor = { 0.7f, 0.9f, 0.85f };
    lightConstants.mMainLightDir = { 0, -1, 0, 1 };
    lightConstants.mShadowColor = { 0.2, 0.2, 0.2, 0.2 };
    lightConstants.mMainLightIntensity = 1;

    Blob lightConstantBuffer{ &lightConstants, sizeof(LightConstant) };

    const std::vector<SubMesh> subMeshes = cubeMesh.subMeshes();
    RenderItem renderItem{};
	renderItem.mMaterial = materialInstance.get();
	renderItem.mMeshData.mVertexBuffer = vertexBufferGPU;
	renderItem.mMeshData.mIndexBuffer = indexBufferGPU;
	renderItem.mMeshData.mVertexCount = cubeMesh.numVertex();
	renderItem.mMeshData.mIndexCount = cubeMesh.numIndex();
    renderItem.mMeshData.mSubMeshCount = 1;
    renderItem.mMeshData.mSubMeshes.reset(new SubMesh[subMeshes.size()]);
    memcpy(renderItem.mMeshData.mSubMeshes.get(), subMeshes.data(), subMeshes.size() * sizeof(SubMesh));

	RenderList renderList{};
    renderList.mCameraConstants.mView = DirectX::XMMatrixLookAtLH({ 0, 5, -10, 1 },
        { 0, 0, 0, 1 },
        { 0, 1, 0, 0 });
    renderList.mCameraConstants.mViewInverse = DirectX::XMMatrixInverse(nullptr, renderList.mCameraConstants.mView);

    renderList.mBackGroundColor = { 0.6902f, 0.7686f, 0.8706f, 1.0f };
    renderList.mCameraConstants.mProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, 1280.0f / 720.0f, 0.3f, 1000.0f);
    renderList.mCameraConstants.mProjectionInverse = DirectX::XMMatrixInverse(nullptr, renderList.mCameraConstants.mProjection);
    renderList.mOpaqueList.push_back(renderItem);
    renderList.mSkyBoxType = SkyboxType::SKYBOX_PROCEDURAL;
    renderList.mSkyboxShader = shaders[1];

    LARGE_INTEGER frequency, start, end, stamp;
    QueryPerformanceFrequency(&frequency);  // 获取计时器频率
    QueryPerformanceCounter(&start);        // 记录起始时间
    stamp = start;

    float deltaTime;

    bool flag = true;
    while (flag)
    {
        MSG msg;
	    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	    {
			if (msg.message == WM_QUIT)
			{
                flag = false;
                break;
			}
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (frame.IsClosed()) PostQuitMessage(0);
	    }

        QueryPerformanceCounter(&end);
        double elapsedTime = static_cast<float>(end.QuadPart - start.QuadPart) / static_cast<float>(frequency.QuadPart);
        deltaTime = static_cast<float>(end.QuadPart - stamp.QuadPart) * 1000.0 / static_cast<float>(frequency.QuadPart);
        stamp = end;
        /*OutputDebugString(std::to_wstring(deltaTime).c_str());
        OutputDebugString(TEXT("\n"));*/
        renderList.mOpaqueList[0].mModel = DirectX::XMMatrixRotationY(static_cast<float>(elapsedTime * 0.18));
        renderList.mOpaqueList[0].mModelInverse = DirectX::XMMatrixInverse(nullptr, renderItem.mModel);
        lightConstants.mMainLightDir.x = std::sin(elapsedTime / 4);
        lightConstants.mMainLightDir.y = -std::cos(elapsedTime / 4);
        renderer.setLightConstants(lightConstants);

        /*renderList.mCameraConstants.mView = DirectX::XMMatrixLookAtLH({ 10 * static_cast<float>(std::sin(elapsedTime / 2)), 0, -10 * static_cast<float>(std::cos(elapsedTime / 2)), 1 },
            { 0, 0, 0, 1 },
            { 0, 1, 0, 0 });
        renderList.mCameraConstants.mViewInverse = DirectX::XMMatrixInverse(nullptr, renderList.mCameraConstants.mView);*/

        std::unique_ptr<RenderList[]> renderLists;
        renderLists.reset(new RenderList[1]{ renderList });
        renderer.setTime(deltaTime, elapsedTime);
        renderer.appendRenderLists(std::move(renderLists), 1);
        renderer.render();
    }

    return 0;
}
