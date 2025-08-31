#include "FileManager.h"

#include "Engine/AudioSystem/AudioClip.h"
#include "Engine/AudioSystem/AudioInterface.h"
#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Engine/render/MeshData.h"
#include "Engine/render/DataCPU/DataParser/RenderResources.h"
#include "Engine/render/Renderer.h"

#include "Engine/Utility/MacroUtility.h"

TpUnorderedMap<TpString, FileManager::BolbFile<unsigned char*>> FileManager::sLoadedBolbFile;
TpUnorderedMap<TpString, FileManager::BolbFile<RenderMeshResource*>> FileManager::sLoadedMeshes;
TpUnorderedMap<TpString, FileManager::BolbFile<RenderTextureResource*>> FileManager::sLoadedTextures;
TpUnorderedMap<TpString, FileManager::BolbFile<std::unique_ptr<Material>>> FileManager::sLoadedShaders;
TpUnorderedMap<TpString, FileManager::BolbFile<AudioData*>> FileManager::sLoadedAudioClips;
bool FileManager::needNessary = false;
std::atomic<bool> FileManager::sIsGpuLoading(false);

namespace TankinRender
{
    void uploadMesh(RenderMeshResource* meshRes)
    {
        //upload mesh data to GPU
        Renderer& renderer = Renderer::GetInstance();
        MeshData& meshDataGpu = meshRes->mMeshDataGpu;
        meshDataGpu.mVertexCount = meshRes->VerticesCPU.NumElements;
        meshDataGpu.mIndexCount = meshRes->IndicesCPU.NumElements;
        meshDataGpu.mSubMeshes.reset(new SubMesh[1]{ {meshDataGpu.mIndexCount , 0, 0}});
        meshDataGpu.mVertexBuffer = renderer.allocVertexBuffer(meshRes->VerticesCPU.GetNumElements(), meshRes->VerticesCPU.GetStride());
        meshDataGpu.mIndexBuffer = renderer.allocIndexBuffer(meshRes->IndicesCPU.GetNumElements(), Format::R32_UINT);
        ASSERT(meshDataGpu.mVertexBuffer.IsValid(), TEXT("Upload Mesh Vertex Failed!"))
        ASSERT(meshDataGpu.mIndexBuffer.IsValid(), TEXT("Upload Mesh Index Failed!"))

        renderer.updateVertexBuffer(meshRes->VerticesCPU.Data, meshRes->VerticesCPU.GetDataBytes(), meshDataGpu.mVertexBuffer);
        renderer.updateIndexBuffer(meshRes->IndicesCPU.Data, meshRes->IndicesCPU.GetDataBytes(), meshDataGpu.mIndexBuffer);
    }
    void uploadTexture(RenderTextureResource* texRes)
    {
        auto& renderer = Renderer::GetInstance();
        texRes->mDataGpuHandle = renderer.allocTexture2D(Format::R8G8B8A8_UNORM_SRGB, texRes->Width, texRes->Height, 1);
        renderer.updateTexture(texRes->RGBATextureDataCPU, texRes->mDataGpuHandle, 0, true);    // 暂时统一设置为阻塞渲染循环，之后提供主动检查任务是否完成的接口
    }
}

template<>
void FileManager::sLoadBolbFile<unsigned char*>(const TpString& fileName, const TpString& filePath,
    bool isAsync)
{
    if (sLoadedBolbFile.find(fileName) != sLoadedBolbFile.end())
        return;
        
    std::ifstream ifs(Application::sGetDataPath() + filePath, std::ios::binary | std::ios::ate);
    ASSERT(ifs.is_open(), TEXT("Failed to open file\n"));

    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);
    unsigned char* data = new unsigned char[size];
    if (!ifs.read(reinterpret_cast<char*>(data), size))
    {
        delete[] data;
        ASSERT(false, TEXT("Failed to read file\n"));
    }
    
    sLoadedBolbFile[fileName] = {data, filePath};
}

template<>
void FileManager::sLoadBolbFile<MeshData>(const TpString& fileName, const TpString& filePath,bool isAsync)
{
    //check whether mesh resource has already been loaded
    auto itor = sLoadedMeshes.find(fileName);
    if (itor != sLoadedMeshes.end())
        return;
    
    std::ifstream ifs(Application::sGetDataPath() + filePath, std::ios::binary);
    ASSERT(!ifs.fail(), TEXT("Mesh file is not found"))
    DEBUG_PRINT("Load mesh <%s> from <%s>\n", fileName.c_str(), filePath.c_str());

    RenderMeshResource* meshRes = new RenderMeshResource();
    bool result = meshRes->LoadMesh(ifs);
    ASSERT(result, TEXT("Failed to load mesh"))

    //--------------------upload resource to GPU--------------------
    if (!isAsync)
    {
#ifdef WIN32
        TankinRender::uploadMesh(meshRes);
#endif
    }

    if (isAsync)
    {
        bool expected = false;
        //compare_exchange_strong make load and store become a atomic sentence, ensure data consistency
        while (!sIsGpuLoading.compare_exchange_strong(expected, true))
        {
            std::this_thread::yield();
        }
    }

    //--------------------upload resource to GPU--------------------
    sLoadedMeshes[fileName] = {meshRes, filePath, !isAsync};
    
    if (isAsync)
    {
        //release gpu resource
        sIsGpuLoading.store(false);
    }
}

template<>
void FileManager::sLoadBolbFile<TextureRef>(const TpString& fileName, const TpString& filePath,bool isAsync)
{
    //check whether mesh resource has already been loaded
    auto itor = sLoadedTextures.find(fileName);
    if (itor != sLoadedTextures.end())
        return;

    std::ifstream ifs(Application::sGetDataPath() + filePath, std::ios::binary);
    ASSERT(!ifs.fail(), TEXT("texture file is not found"))
    DEBUG_PRINT("Load texture <%s> from <%s>\n", name.c_str(), filePath.c_str());

    RenderTextureResource* texRes = new RenderTextureResource();
    
    bool result = texRes->LoadTGATexture(ifs);
    ASSERT(result, TEXT("Failed to load texture"))

    //--------------------upload resource to GPU--------------------
    
    if (isAsync)
    {
        bool expected = false;
        //compare_exchange_strong make load and store become a atomic sentence, ensure data consistency
        while (!sIsGpuLoading.compare_exchange_strong(expected, true))
        {
            std::this_thread::yield();
        }
    }
    else
    {
        TankinRender::uploadTexture(texRes);
    }

    sLoadedTextures[fileName] = {texRes, filePath, !isAsync};

    if (isAsync)
    {
        //release gpu resource
        sIsGpuLoading.store(false);
    }
}

template<>
void FileManager::sLoadBolbFile<ShaderRef>(const TpString& fileName, const TpString& filePath, bool isAsync)
{
    auto itor = sLoadedShaders.find(fileName);
    if (itor != sLoadedShaders.end())
        return;
    // 1. 打开文件（二进制模式）
    TpString finalPath;
#ifdef WIN32
    finalPath = Application::sGetDataPath() + "PcShader/" + filePath;
    std::ifstream file(finalPath, std::ios::binary | std::ios::ate);
#elif defined(ORBIS)
    finalPath = Application::sGetDataPath() + "PcShader/" + filePath;
    std::ifstream file(finalPath, std::ios::binary | std::ios::ate);
#endif
    ASSERT(file.is_open(), TEXT("Failed to open file\n"));

    // 2. 获取文件大小
    std::streampos fileSize = file.tellg();
    if (fileSize == -1) {
        throw Exception(AsciiToUtf8(TpString("Failed to get file size: ")+filePath).c_str());
    }

    // 3. 分配内存（调用方需负责释放！）
    std::unique_ptr<char[]> binShader = std::make_unique<char[]>(fileSize);

    // 4. 读取文件内容
    file.seekg(0, std::ios::beg);
    if (!file.read(binShader.get(), fileSize)) {
        binShader.release();
        binShader = nullptr;
        throw Exception(AsciiToUtf8(TpString("Failed to read file: ")+filePath).c_str());
    }

    Blob shaderBinary {std::move(binShader), static_cast<size_t>(fileSize)};
    Renderer& renderer = Renderer::GetInstance();
    ShaderRef shaderRef =  renderer.compileAndRegisterShader(fileName, shaderBinary,
                                             static_cast<ShaderType>(ShaderType::VERTEX |
                                                 ShaderType::PIXEL), &finalPath);
    
    BolbFile<std::unique_ptr<Material>> bolbFile;
    bolbFile.filePath = filePath;
    bolbFile.isUploadGpu = true;
    bolbFile.data = Renderer::createMaterial(shaderRef.Get());
    
    if (isAsync)
    {
        bool expected = false;
        //compare_exchange_strong make load and store become a atomic sentence, ensure data consistency
        while (!sIsGpuLoading.compare_exchange_strong(expected, true))
        {
            std::this_thread::yield();
        }
    }
    
    sLoadedShaders[fileName] = std::move(bolbFile);
    
    if (isAsync)
    {
        //release gpu resource
        sIsGpuLoading.store(false);
    }
}
template<>
void FileManager::sLoadBolbFile<AudioClip*>(const TpString& fileName, const TpString& filePath,bool isAsync)
{
    auto itor = sLoadedAudioClips.find(fileName);
    if (itor != sLoadedAudioClips.end()) 
        return;
    
    std::ifstream ifs(Application::sGetDataPath() + filePath, std::ios::binary);
    ASSERT(!ifs.fail(), TEXT("Audio file not found!"))
    AudioData* audioData = new AudioData();
    audioData->ReadFile(ifs);
    BolbFile<AudioData*> bolbFile;
    bolbFile.data = audioData;
    bolbFile.filePath = filePath;
    
    if (isAsync)
    {
        bool expected = false;
        //compare_exchange_strong make load and store become a atomic sentence, ensure data consistency
        while (!sIsGpuLoading.compare_exchange_strong(expected, true))
        {
            std::this_thread::yield();
        }
    }
    
    sLoadedAudioClips[fileName] = bolbFile;

    if (isAsync)
    {
        //release gpu resource
        sIsGpuLoading.store(false);
    }
    
    return;
}

void FileManager::uploadAllAssets()
{
    for (auto& file : sLoadedMeshes)
    {
        if (!file.second.isUploadGpu)
        {
            TankinRender::uploadMesh(file.second.data);
            file.second.isUploadGpu = true;
        }
    }

    for (auto& file : sLoadedTextures)
    {
        if (!file.second.isUploadGpu)
        {
            TankinRender::uploadTexture(file.second.data);
            file.second.isUploadGpu = true;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template <>
Material* FileManager::sGetLoadedBolbFile<Material*>(const TpString& fileName)
{
    //返回原始Material指针，外部使用Material创建MaterialInstance
    auto itor = sLoadedShaders.find(fileName);
    ASSERT(itor != sLoadedShaders.end(), TEXT("Shader not found!"))
    return itor->second.data.get();
}

template<>
MeshData FileManager::sGetLoadedBolbFile<MeshData>(const TpString& fileName)
{
    auto itor = sLoadedMeshes.find(fileName);
    ASSERT(itor != sLoadedMeshes.end(), TEXT("Mesh not found!"))
    return itor->second.data->mMeshDataGpu;
}

template<>
TextureRef FileManager::sGetLoadedBolbFile<TextureRef>(const TpString& fileName)
{
    auto itor = sLoadedTextures.find(fileName);
    ASSERT(itor != sLoadedTextures.end(), TEXT("Texture not found!"))
    return itor->second.data->mDataGpuHandle;
}

template<>
unsigned char* FileManager::sGetLoadedBolbFile<unsigned char*>(const TpString& fileName)
{
    auto itor = sLoadedBolbFile.find(fileName);
    ASSERT(itor != sLoadedBolbFile.end(), TEXT("bolbfile not found!"))
    return itor->second.data;
}

template<>
AudioClip* FileManager::sGetLoadedBolbFile<AudioClip*>(const TpString& fileName)
{
    auto itor = sLoadedAudioClips.find(fileName);
    ASSERT(itor != sLoadedAudioClips.end(), TEXT("bolbfile not found!"))
    return AudioInterface::sGetInstance()->CreatAudioResource(itor->second.data);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FileManager::serializeSelf(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father)
{
    auto node = doc->allocate_node(rapidxml::node_element, "FileManager");
    father->append_node(node);
    node->append_attribute(
        doc->allocate_attribute(
            "NeedAsync",
            doc->allocate_string(std::to_string(true).c_str())
            )
            );
    
    auto childNode = doc->allocate_node(rapidxml::node_element, "BolbFile");
    node->append_node(childNode);
    serilizeOneMap<unsigned char*>(doc, childNode, sLoadedBolbFile);

    childNode = doc->allocate_node(rapidxml::node_element, "Meshes");
    node->append_node(childNode);
    serilizeOneMap<RenderMeshResource*>(doc, childNode, sLoadedMeshes);

    childNode = doc->allocate_node(rapidxml::node_element, "Textures");
    node->append_node(childNode);
    serilizeOneMap<RenderTextureResource*>(doc, childNode, sLoadedTextures);

    childNode = doc->allocate_node(rapidxml::node_element, "Shaders");
    node->append_node(childNode);
    serilizeOneMap<std::unique_ptr<Material>>(doc, childNode, sLoadedShaders);

    childNode = doc->allocate_node(rapidxml::node_element, "AudioClips");
    node->append_node(childNode);
    serilizeOneMap<AudioData*>(doc, childNode, sLoadedAudioClips);

    childNode = doc->allocate_node(rapidxml::node_element, "NecessaryFile");
    node->append_node(childNode);
    
    childNode->append_attribute(doc->allocate_attribute("need", doc->allocate_string(std::to_string(needNessary).c_str())));
    childNode->append_attribute(
        doc->allocate_attribute(
            "NeedAsync",
            doc->allocate_string(std::to_string(true).c_str())
            )
            );
}

void FileManager::deserializeSelf(rapidxml::xml_node<>* node, TpList<std::future<void>>& tasks)
{
    
    auto childNode = node->first_node("BolbFile");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<unsigned char*>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value())
            ,tasks);
    }

    childNode = node->first_node("Meshes");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<MeshData>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }

    childNode = node->first_node("Textures");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<TextureRef>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }
    
    childNode = node->first_node("Shaders");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<ShaderRef>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }

    childNode = node->first_node("AudioClips");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<AudioClip*>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }

    childNode = node->first_node("NecessaryFile");

    needNessary = std::stoi(childNode->first_attribute("need")->value());

    //auto tempmesh = sLoadedMeshes;
    //auto tempshader = sLoadedShaders;
    //auto temptexture = sLoadedTextures;
}

void FileManager::sLoadNessaryFile(rapidxml::xml_node<>* node)
{
    needNessary = true;
    TpList<std::future<void>> tasks;
    auto childNode = node->first_node("BolbFile");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<unsigned char*>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value())
            ,tasks);
    }

    childNode = node->first_node("Meshes");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<MeshData>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }

    childNode = node->first_node("Textures");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<TextureRef>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }
    
    childNode = node->first_node("Shaders");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<ShaderRef>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }

    childNode = node->first_node("AudioClips");
    if (childNode != nullptr)
    {
        deSerilizeOneMap<AudioClip*>(
            childNode,std::stoi(node->first_attribute("NeedAsync")->value()),
            tasks);
    }
    
    //等待加载完毕
    for (auto& task:tasks)
        task.wait();
    FileManager::uploadAllAssets();
}
