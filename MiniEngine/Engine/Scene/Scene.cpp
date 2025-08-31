#include "Scene.h"
#include <chrono>

#include "Engine/Application.h"
#include "Engine/AISystem/NavigationMap.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Memory/TankinMemory.h"

#include "Engine/Component/Transform.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Engine/Dependencies/tga/tga.h"
#include "Engine/FileManager/FileManager.h"


bool Scene::sNeedLoadScene = true;
TpString Scene::sSceneFilePath = "Assets/Scene/StartScene.xml";
bool Scene::sDirectEnterGamePlay = false;
rapidxml::xml_node<>* Scene::sFileManagerNode = nullptr;

void Scene::sLoadScene()
{
    
    // 地图设置
    NavigationMap* navMap = NavigationMap::getInstance();
    if (!navMap->isLoaded())
    {
        MapData& mapData = MapData::getInstance();
        mapData.loadFromFile(Application::sGetDataPath()+"AIMap/level.txt");
        navMap->setAiMap(&mapData);
    }
    else
    {
        MapData& mapData = MapData::getInstance();
        mapData.clearMap();
        navMap->setAiMap(&mapData);
    }
    
    deSerialize(sSceneFilePath);
    
    sNeedLoadScene = false;
}

void Scene::reLoadScene()
{
    GameObjectFactory::sDestroyScene();
    sNeedLoadScene = true;
}

void Scene::sSaveScene()
{
    auto doc = new rapidxml::xml_document<>();
    auto root = doc->allocate_node(rapidxml::node_element, "Scene");
    doc->append_node(root);
    Transform* transformRoot = Transform::sGetRoot();
    transformRoot->foreachPreorder([doc, root](const Transform* thisTransform)
    {
        Transform* temp = const_cast<Transform*>(thisTransform);
        ISerializable* serializable = static_cast<ISerializable*>(temp);
        Transform* parent = thisTransform->getParent();
        if (parent == nullptr)
        {
            serializable->mXmlNode = thisTransform->getGameObject()->serialize(doc, root);
        }
        else
        {
            ISerializable* parentSerializable = static_cast<ISerializable*>(parent);
            serializable->mXmlNode = thisTransform->getGameObject()->serialize(doc, parentSerializable->mXmlNode);
        }
    });

    FileManager::serializeSelf(doc,root);
    
    std::ofstream file(sSceneFilePath);
    ASSERT(file.is_open(), TEXT("can not open file!"))
    file << (*doc);
    
    file.close();
    SAFE_DELETE_POINTER(doc);
}

void Scene::deSerialize(const TpString& fileName)
{
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    //deSerialize FileManager
    rapidxml::file<>* xmlFile = new rapidxml::file<>(fileName.c_str());
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

    doc->parse<0>(xmlFile->data());
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
    uint64_t microSecond = std::chrono::duration_cast<std::chrono::milliseconds>(current-start).count();
    start = current;
    DEBUG_FOCUS_PRINT("Load scene xml: %lf\n", microSecond/1000.0)

    rapidxml::xml_node<>* rootnode = doc->first_node("Scene");

    //解析并加载资源
    sFileManagerNode = rootnode->first_node("FileManager");
    auto nessaryNode = sFileManagerNode->first_node("NecessaryFile");
    int need = std::stoi(nessaryNode->first_attribute("need")->value());
    if (need)
    {
        rapidxml::file<>* xmlNecessaryFile = new rapidxml::file<>((Application::sGetDataPath()+"Configuration/NecessaryFile.xml").c_str());
        rapidxml::xml_document<>* necessaryDoc = new rapidxml::xml_document<>();
        necessaryDoc->parse<0>(xmlNecessaryFile->data());
        FileManager::sLoadNessaryFile(necessaryDoc->first_node("NecessaryFile"));
    }
    else
    {
        TpList<std::future<void>> tasks;
        FileManager::deserializeSelf(sFileManagerNode, tasks);
        //等待加载完毕
        for (auto& task:tasks)
            task.wait();
        FileManager::uploadAllAssets();
    }

    current = std::chrono::high_resolution_clock::now();
    microSecond = std::chrono::duration_cast<std::chrono::milliseconds>(current-start).count();
    start = current;
    DEBUG_FOCUS_PRINT("Load Assets: %lf\n", microSecond/1000.0)

    
    //deSerialize GameObject
    //root should not mount any component

    //创建场景中的物体
    rapidxml::xml_node<>* GameObjectRoot = rootnode->first_node("GameObject");
    Transform::sGetRoot()->getGameObject()->deSerialize(GameObjectRoot);

    current = std::chrono::high_resolution_clock::now();
    microSecond = std::chrono::duration_cast<std::chrono::milliseconds>(current-start).count();
    start = current;
    DEBUG_FOCUS_PRINT("Creat GameObject: %lf\n", microSecond/1000.0)
}