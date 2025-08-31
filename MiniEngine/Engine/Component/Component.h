#pragma once

#include <functional>
#include <type_traits>

#include "Engine/Utility/MacroUtility.h"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/common/Exception.h"
#include "Engine/Editor/IEditable.h"
#include "Engine/Scene/ISerializable.h"

class GameObject;
class ComponentFactory;

class Component : public ISerializable, public IEditable
{
    friend class ComponentFactory;
    friend class GameObject;
    friend class ComponentRegister;
    friend class GameObjectFactory;
public:
    friend class GameObjectFactory;
    GameObject* getGameObject()const;
    std::string getGameObjectName()const;
    DELETE_CONSTRUCTOR_FIVE(Component)

    void setGameObject(GameObject* gameObject) {mGameObject = gameObject;}

    //Initialization
    virtual void awake();
    virtual void onEnable();
    virtual void start();

    //Physics
    virtual void fixedUpdate();

    //Gamelogic
    virtual void update();

    //Disable
    virtual void onDisable();

    //Decomissioning
    virtual void onDestory();

    //attach transform component
    static void sUpdateAllComponent();
    static void sFixedUpdateAllComponent();
    static void sAwakeAllMonoBehavior();
    
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value = "") override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
    void showSelf() override;
    
protected:
    Component();
    virtual ~Component();

    bool isFirstUpdate = true;

    GameObject* mGameObject = nullptr;
};

class ComponentFactory
{
public:
    friend class GameObject;
    friend class ComponentRegister;
    friend class Component;
    friend class GameObjectFactory;
    ~ComponentFactory() = default;
    static void sGarbageCollect();
    
private:
    DELETE_CONSTRUCTOR_FIVE(ComponentFactory)
    
    static Component* sCreateComponent(const std::string& name);
    ///Destroy Component will push it to garbagelist, and will be collected in the end of frame
    static void sDestroyComponent(Component* component);
    static TpList<Component*> sGarbageList;
    ComponentFactory() = default;
    
};

///reflection
class ComponentRegister
{
public:
    friend class ComponentFactory;
    friend class GameObject;
    friend class Component;
    friend class GameObjectFactory;
    //an assistant class, helping register component class
    template<class T>
    class Register
    {
    public:
        Register(const TpString& name)
        {
            ASSERT((std::is_base_of<Component, T>::value), TEXT("class T is not derived from Component!"));
            sGetInstance()->registerClass(name,[](){return new T();});
        }
    };
    
    using CreateFunction = std::function<Component*()>;

    static ComponentRegister* sGetInstance()
    {
        if (sInstance == nullptr)
        {
            sInstance = new ComponentRegister();
        }
        return sInstance;
    }

    static TpString sGetClassName(const Component* cm)
    {
        auto name = typeid(*cm).name();
        // the name is [class componentName], so need +6
        const char* newName = (name + 6);
        return newName;
    }
    
private:
    ComponentRegister() = default;
    static ComponentRegister* sInstance;
    void registerClass(const TpString& name, const CreateFunction& func)
    {
        auto itor = registry.find(name);
        ASSERT(itor == registry.end(), TEXT("Component class name already registered!"));
        registry[name] = func;
    }
    TpUnorderedMap<TpString, CreateFunction> registry;
};

#define REGISTER_COMPONENT(component, name)\
     static ComponentRegister::Register<component> componentRegister(name);