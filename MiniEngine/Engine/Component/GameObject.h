#pragma once

#include <functional>
#include <memory>

#include "Layer.h"
#include "Engine/Editor/IEditable.h"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/Scene/ISerializable.h"
#include "Engine/Utility/MacroUtility.h"

class Canvas;
class Component;
class Transform;

#define MAX_GAMEOBJECT_NAME_LENGTH 50
#define MAX_GAMEOBJECT_TAG_LENGTH 20

///please use pointer to create GameObject if it has transform component
class GameObject final : public ISerializable, public IEditable
{
    friend class GameObjectFactory;
    friend class Transform;
    friend class Component;
public:
    //Serialization
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "") override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    
    void showSelf() override;
    
    DELETE_CONSTRUCTOR_FIVE(GameObject)
    
    TpString getName()const { return mName; }
    void setName(const TpString& name){mName = name;}
    
    Transform* getTransform() const{ return mTransform; }
    void addTransform();
    void addTransform(Transform* parent);
    void addRectTransform(Transform* parent, Canvas* canvas);

    //attach other component
    Component* addComponent(const TpString& componentName);
    void removeComponent(const TpString& componentName);
    Component* getComponent(const TpString& componentName);
    TpUnorderedMap<std::string, Component*>* getAllComponents() {return &mComponents;}

    //layer
    void setLayer(const Layer layer);
    unsigned char getLayer() const;

    //active
    void activeGameObject();
    void deactiveGameObject();
    bool isActive() const;

    //Debug
    void printSelf() const;

    //Tag
    TpString getTag() const { return mTag; }
    void setTag(const TpString& tag){mTag = tag;}

private:
    //GameObject only can create and delete by GameObject Factory
    GameObject();
    GameObject(const TpString& name);
    GameObject(const TpString& name, Transform* parent);
    ~GameObject();

    //for load scene
    ///this function will not call monobehavior awake and onEnable, only system component will be called
    Component* addComponentWithoutCalling(const TpString& componentName);
    
    TpString mName;
    Transform* mTransform = nullptr;
    TpUnorderedMap<std::string, Component*> mComponents;

    unsigned char mLayer = static_cast<unsigned char>(Layer::LAYER_Default); 

    void setActive(bool active) {mActive = active;}
    bool mActive = true;
    TpString mTag = "Common";

    //防止重复destroy引起crash
    bool isWillDestroy = false;
};

class GameObjectFactory
{
public:
    friend class GameObject;
    DELETE_CONSTRUCTOR_FIVE(GameObjectFactory)
    ~GameObjectFactory() = default;
    static GameObject* sCreateGameObject(const TpString& name = TpString());
    static void sRegisterDestroyCallBack(const std::function<void(GameObject* thisGo)>& func)
    {
        DestroyCallBackList.push_back(func);
    }
    static void sRegisterCreareCallBack(const std::function<void(GameObject* thisGo)>& func)
    {
        CreateCallBackList.push_back(func);
    }
    static void sDestroyGameObject(GameObject*& go);
    static void sGarbageCollect();
    static void sDestroyScene();
    
    //store all GameObject, avoid memory leak because of some GameObject without transform
    static TpUnorderedSet<GameObject*> sGameObjects;
    //Destroy gameobject will push to this list, and will be collected in the end of frame
    static TpList<GameObject*> sGarbageList;
private:
    GameObjectFactory() = default;
    static TpList<std::function<void(GameObject* thisGo)>> DestroyCallBackList;
    static TpList<std::function<void(GameObject* thisGo)>> CreateCallBackList;
};
