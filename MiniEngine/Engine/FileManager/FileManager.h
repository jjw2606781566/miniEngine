#pragma once
#include <fstream>

#include "Engine/Application.h"
#include "Engine/common/Exception.h"
#include "Engine/Dependencies/rapidxml/rapidxml.hpp"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/Render/Material.h"
#include "Engine/Render/RenderResource.h"
#include "Engine/Utility/ThreadPool/ThreadPool.h"

struct AudioData;
class AudioResourceWave;
struct MaterialCPU;
struct MeshData;
class RenderTextureResource;
class RenderMeshResource;

class FileManager
{
public:
    template<class T>
    static T sGetLoadedBolbFile(const TpString& fileName);
    ///Load Bolb File which user do not want to control its life cycle,
    template<class T>
    static void sLoadBolbFile(const TpString& fileName, const TpString& filePath, bool isAsync = false);
    
    template<class T>
    static std::future<void> sLoadBolbFileAsync(const TpString& fileName, const TpString& filePath)
    {
        auto threadPool = Application::sGetThreadPool();
        
        auto task = threadPool->enqueue(1,[fileName, filePath]()
        {
            sLoadBolbFile<T>(fileName, filePath, true);
        });
        return task;
    }
    
    static unsigned char* sLoadBinFile(const TpString& filePath)
    {
        std::ifstream ifs(filePath, std::ios::binary | std::ios::ate);
        ASSERT(ifs.is_open(), TEXT("Failed to open file\n"));

        std::streamsize size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        unsigned char* data = new unsigned char[size];
        if (!ifs.read(reinterpret_cast<char*>(data), size))
        {
            delete[] data;
            ASSERT(false, TEXT("Failed to read file\n"));
        }
        
        return data;
    }
    static void sLoadBinFileAsync(const TpString& filePath,
        const std::function<void(unsigned char*)>& callBack)
    {
        auto threadPool = Application::sGetThreadPool();
        auto task = threadPool->enqueue(1,[&]()
        {
            unsigned char* data = nullptr;
            data = sLoadBinFile(filePath);
            callBack(data);
        });
    }
    static void sLoadBatchBinFileAsync(const TpVector<TpString>& filePathList
        , const TpVector<std::function<void(unsigned char*)>>& callBackList,
        const std::function<void()> allFinishedCallBack)
    {
        auto threadPool = Application::sGetThreadPool();
        auto task = threadPool->enqueue(1,[&]()
        {
            unsigned char* data = nullptr;
            for (size_t i=0; i<filePathList.size(); i++)
            {
                data = sLoadBinFile(filePathList[i]);
                callBackList[i](data);
            }
            allFinishedCallBack();
        });
    }
    template<class T>
    struct BolbFile
    {
        T data;
        TpString filePath;
        bool isUploadGpu = false;
    };
    static void serializeSelf(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father);
    static void deserializeSelf(rapidxml::xml_node<>* node, TpList<std::future<void>>& tasks);
    static void sLoadNessaryFile(rapidxml::xml_node<>* node);
    static void uploadAllAssets();
private:
    //gpu uploading is synchronous
    template<class MapType>
    static void serilizeOneMap(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father
        , TpUnorderedMap<TpString, BolbFile<MapType>>& map)
    {
        for (auto& itor: map)
        {
            auto fileNode = doc->allocate_node(rapidxml::node_element, "File");
            fileNode->append_attribute(doc->allocate_attribute("name", itor.first.c_str()));
            fileNode->append_attribute(doc->allocate_attribute("path", itor.second.filePath.c_str()));
            father->append_node(fileNode);
        }
    }
    template<class FileType>
    static void deSerilizeOneMap(rapidxml::xml_node<>* node, bool isAsync, TpList<std::future<void>>& tasks)
    {
        auto fileNode = node->first_node("File");
        while (fileNode != nullptr)
        {
            if (isAsync)
            {
                tasks.push_back(
                    sLoadBolbFileAsync<FileType>(
                    fileNode->first_attribute("name")->value()
                , fileNode->first_attribute("path")->value()));
            }
            else
            {
                sLoadBolbFile<FileType>(
                    fileNode->first_attribute("name")->value()
                    , fileNode->first_attribute("path")->value());
            }
            fileNode = fileNode->next_sibling("File");
        }
    }
    static std::atomic<bool> sIsGpuLoading;
    static TpUnorderedMap<TpString, BolbFile<unsigned char*>> sLoadedBolbFile;
    static TpUnorderedMap<TpString, BolbFile<RenderMeshResource*>> sLoadedMeshes;
    static TpUnorderedMap<TpString, BolbFile<RenderTextureResource*>> sLoadedTextures;
    static TpUnorderedMap<TpString, BolbFile<std::unique_ptr<Material>>> sLoadedShaders;
    static TpUnorderedMap<TpString, BolbFile<AudioData*>> sLoadedAudioClips;
    static bool needNessary;
};
