#pragma once
#include "Engine/Dependencies/rapidxml/rapidxml_ext.h"
#include "Engine/Memory/TankinMemory.h"

///load and save scene from file
class Scene
{
    friend class Application;
public:
    static void sSaveScene();
    static rapidxml::xml_node<>* sGetFileManagerNode(){return sFileManagerNode;}
private:
    static void sLoadScene();
    static void reLoadScene();
    static void deSerialize(const TpString& fileName);
    static bool sNeedLoadScene;
    static bool sDirectEnterGamePlay;
    static TpString sSceneFilePath;
    //用于多线程加载传递node
    static rapidxml::xml_node<>* sFileManagerNode;
};