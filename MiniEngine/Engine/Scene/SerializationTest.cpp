#include "Scene.h"
#include "Engine/Application.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"
#include "Engine/Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Engine/FileManager/FileManager.h"

using namespace std;

#ifdef WIN32
#include <Windows.h>

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Application::sSetDataPath("Assets/");
    auto test1 = GameObjectFactory::sCreateGameObject("Test");
    auto test2 = GameObjectFactory::sCreateGameObject("Test2");

    test1->addTransform();
    test2->addTransform();

    test1->getTransform()->setLocalPosition({15.0f, 11, 0});
    test1->getTransform()->setLocalRotation({11,22,33,44});
    test1->getTransform()->setLocalScale({1,1,1});
    test1->getTransform()->setParent(test2->getTransform());

    Scene::sSaveScene();
}*/

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Application::sSetDataPath("Assets/");
    rapidxml::file<>* xmlFile = new rapidxml::file<>("Assets/Scene/DefaultScene.xml");
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

    doc->parse<0>(xmlFile->data());

    rapidxml::xml_node<>* rootnode = doc->first_node("Scene");
    //root do not need parse, and root should not mount any component
    rapidxml::xml_node<>* GameObjectRoot = rootnode->first_node("GameObject");
    Transform::sGetRoot()->getGameObject()->deSerialize(GameObjectRoot);

    auto root = Transform::sGetRoot();
    
    Transform::sGetRoot()->printAll();
}*/

/*int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Application::sSetDataPath("Assets/");
    Application::sInit();
    rapidxml::file<>* xmlFile = new rapidxml::file<>("Assets/Scene/DefaultScene.xml");
    rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

    doc->parse<0>(xmlFile->data());

    rapidxml::xml_node<>* rootnode = doc->first_node("Scene");
    auto fileManagerNode = rootnode->first_node("FileManager");
    FileManager::deserializeSelf(fileManagerNode);
}*/

#endif