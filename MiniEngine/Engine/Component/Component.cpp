#include "GameObject.h"
#include "Component.h"

#include "MonoBehavior.h"
#include "Transform.h"
#include "RenderComponent/Camera.h"

#include "Engine/common/Exception.h"
#include "Engine/Utility/GameTime/GameTime.h"

Component::Component()
{
    mGameObject = nullptr;
}

Component::~Component()
{
    mGameObject = nullptr;
}

rapidxml::xml_node<>* Component::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);
    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string(ComponentRegister::sGetClassName(this).c_str())));

    return mXmlNode;
}

void Component::deSerialize(const rapidxml::xml_node<>* node)
{
}

void Component::showSelf()
{
#ifdef WIN32
    ImGuiTreeNodeFlags baseFlags =
        ImGuiTreeNodeFlags_SpanAvailWidth
        | ImGuiTreeNodeFlags_Leaf
        | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    ImGui::TreeNodeEx(ComponentRegister::sGetClassName(this).c_str(), baseFlags);
#endif
}

void Component::onEnable()
{
}

void Component::awake()
{
}

void Component::start()
{
}

void Component::fixedUpdate()
{
}

void Component::update()
{
}

void Component::onDisable()
{
}

void Component::onDestory()
{
    
}

void Component::sUpdateAllComponent()
{
    Transform* root = Transform::sGetRoot();
    std::function<void(const Transform* transform)> func =
        [](const Transform* transform)
        {
            GameObject* go = transform->getGameObject();
            if (go->mTransform->isFirstUpdate)
            {
                go->mTransform->start();
                go->mTransform->isFirstUpdate = false;
            }
            go->mTransform->update();
            for (auto& cm: go->mComponents)
            {
                if (dynamic_cast<MonoBehavior*>(cm.second) != nullptr && Application::sGetRunningType() == EngineRunningType::Editor)
                {
                    continue;
                }
                    
                if (cm.second->isFirstUpdate)
                {
                    cm.second->start();
                    cm.second->isFirstUpdate = false;
                }
                cm.second->update();
            }
        };
    root->foreachActivePreorder(func);
}

void Component::sFixedUpdateAllComponent()
{
    uint64_t updateCount =  GameTime::sGetFixedUpdateCount();
    Transform* root = Transform::sGetRoot();
    std::function<void(const Transform* transform)> func =
        [](const Transform* transform)
        {
            GameObject* go = transform->getGameObject();
            go->mTransform->fixedUpdate();
            for (auto& cm: go->mComponents)
            {
                //only update system Component
                if (dynamic_cast<MonoBehavior*>(cm.second) != nullptr && Application::sGetRunningType() == EngineRunningType::Editor)
                    continue;
                cm.second->fixedUpdate();
            }
        };
    for (int i=0; i<updateCount; i++)
        root->foreachActivePreorder(func);
    DEBUG_PRINT("FixedUpdateCount: %llu\n", updateCount);
    DEBUG_PRINT("DeltaTime: %lf\n", GameTime::sGetDeltaTime());
}

void Component::sAwakeAllMonoBehavior()
{
    //calling all MonoBehavior's awake and onEnable
    auto root = Transform::sGetRoot();
    root->foreachPreorder([](const Transform* thisTransform)
    {
        GameObject* go = thisTransform->getGameObject();
        for (auto& cm: *(go->getAllComponents()))
        {
            if (dynamic_cast<MonoBehavior*>(cm.second) != nullptr)
            {
                cm.second->awake();
                cm.second->onEnable();
            }
        }
    });
}


GameObject* Component::getGameObject() const
{
    return mGameObject;
}

std::string Component::getGameObjectName() const
{
    if (mGameObject == nullptr)
        return "";
    return mGameObject->getName();
}


ComponentRegister* ComponentRegister::sInstance = nullptr;
TpList<Component*> ComponentFactory::sGarbageList;

Component* ComponentFactory::sCreateComponent(const std::string& name)
{
    if (name == "Transform" || name == "RectTransform")
    {
        ASSERT(false,
            TEXT("Please Use GameObject Instance Method to attach Transform Component"));
    }

    auto registry = ComponentRegister::sGetInstance()->registry;
    
    auto it = registry.find(name);
    if (it != registry.end())
    {
        return (it->second)();
    }
    ASSERT(false,TEXT("Unknown component name!"))
}

void ComponentFactory::sDestroyComponent(Component* component)
{
    if (Application::sGetRunningType() != Editor)
    {
        component->onDisable();
        component->onDestory();
    }
    sGarbageList.push_back(component);
}

void ComponentFactory::sGarbageCollect()
{
    for (auto& cm: sGarbageList)
    {
        SAFE_DELETE_POINTER(cm);
    }
    sGarbageList.clear();
}