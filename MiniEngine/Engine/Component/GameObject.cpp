#include <queue>
#include <iostream>

#include "GameObject.h"

#include "MonoBehavior.h"
#include "Transform.h"
#include "Engine/common/Exception.h"
#include "TGUI/Canvas.h"
#include "TGUI/RectTransform.h"


GameObject::GameObject():mName("NewGameObject")
{
    //keep 100 bucket, avoiding  Triggering expansion and cause iterators to fail
    mComponents.reserve(100);
}

GameObject::GameObject(const TpString& name):mName(name)
{
    
}

///this constructor will automatically attach transform component
GameObject::GameObject(const TpString& name, Transform* parent):mName(name)
{
    ASSERT(parent != nullptr, TEXT("parent is nullptr"));
    Transform* ts = new Transform(this, parent);
    mTransform = ts;
    ts->awake();
    ts->onEnable();
}

GameObject::~GameObject()
{
    
}

Component* GameObject::addComponentWithoutCalling(const TpString& componentName)
{
    ASSERT(componentName != "Transform" && componentName != "RectTransform",
        TEXT("Please Use GameObject Instance Method <addTransform> to attach Transform Component"))

    ASSERT(mComponents.find(componentName) == mComponents.end(),
        TEXT("Component has already existed in this GameObject"))
    
    Component* cm = ComponentFactory::sCreateComponent(componentName);
    ASSERT(cm != nullptr, TEXT("ComponentFactory return a nullptr"))

    //bind go
    cm->mGameObject = this;
    
    mComponents[componentName] = cm;

    //do not call awake and onEnable
    if (dynamic_cast<MonoBehavior*>(cm) == nullptr)
    {
        cm->awake();
        cm->onEnable();
    }

    return cm;
}

rapidxml::xml_node<>* GameObject::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "GameObject");
    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string(mName.c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("layer", doc->allocate_string(std::to_string(mLayer).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("active", doc->allocate_string(std::to_string(mActive).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("tag", doc->allocate_string(mTag.c_str())));
    father->append_node(mXmlNode);

    if (mTransform != nullptr)
    {
        mTransform->serialize(doc, mXmlNode);
    }

    for (auto& cm: mComponents)
    {
        cm.second->serialize(doc, mXmlNode);
    }

    return mXmlNode;
}

void GameObject::deSerialize(const rapidxml::xml_node<>* node)
{
    mName = node->first_attribute("name")->value();
    mLayer = static_cast<unsigned char>(std::stoi(node->first_attribute("layer")->value()));
    mActive = static_cast<bool>(std::stoi(node->first_attribute("active")->value()));
    mTag = node->first_attribute("tag")->value();
    
    auto ComponentNode = node->first_node("Component");
    while (ComponentNode != nullptr)
    {
        TpString temp = ComponentNode->first_attribute("name")->value();
        if (strcmp(ComponentNode->first_attribute("name")->value(), "Transform") == 0)
        {
            if (mTransform == nullptr)
            {
                addTransform();
                mTransform->deSerialize(ComponentNode);
            }
        }
        else if (strcmp(ComponentNode->first_attribute("name")->value(), "RectTransform") == 0 && mTransform == nullptr)
        {
            //UI must under canvas
            addRectTransform(Canvas::sGetCurrentCanvas()->getTransform(),Canvas::sGetCurrentCanvas());
            mTransform->deSerialize(ComponentNode);
        }
        else
        {
            Component* component = addComponentWithoutCalling(ComponentNode->first_attribute("name")->value());
            component->deSerialize(ComponentNode);
        }
        ComponentNode = ComponentNode->next_sibling("Component");
    }

    //Deserialize its children
    auto childNode = node->first_node("GameObject");
    while (childNode != nullptr)
    {

        GameObject* go = GameObjectFactory::sCreateGameObject("");
        go->deSerialize(childNode);
        go->getTransform()->setParent(mTransform);
        childNode = childNode->next_sibling("GameObject");
    }
}

void GameObject::showSelf()
{
#ifdef WIN32
    static char* currentName = new char[MAX_GAMEOBJECT_NAME_LENGTH];
    strcpy_s(currentName, MAX_GAMEOBJECT_NAME_LENGTH, mName.c_str());
    if (ImGui::InputText("Name", currentName, MAX_GAMEOBJECT_NAME_LENGTH))
    {
        setName(currentName);
    }
    static char* currentTag = new char[MAX_GAMEOBJECT_TAG_LENGTH];
    strcpy_s(currentTag, MAX_GAMEOBJECT_TAG_LENGTH, mTag.c_str());
    if (ImGui::InputText("Tag", currentTag, MAX_GAMEOBJECT_TAG_LENGTH))
    {
        setTag(currentTag);
    }
    
    bool isActivate = mActive;
    if (ImGui::Checkbox("Active", &isActivate))
    {
        setActive(isActivate);
    }
    
    const char** layerNames = LayerUtility::sGetInstance()->getLayerNames();
    int currentLayerItem = LayerUtility::sGetInstance()->getLayerNumber(static_cast<Layer>(mLayer));

    if (ImGui::Combo("Layer", &currentLayerItem, layerNames, static_cast<int>(Layer::LAYER_NUM)))
    {
        setLayer(static_cast<Layer>(currentLayerItem));
    }

    if (mTransform != nullptr)
    {
        mTransform->showSelf();
    }

    for (auto& cm:mComponents)
    {
        cm.second->showSelf();
    }
#endif
}

void GameObject::addTransform()
{
    ASSERT(mTransform == nullptr, TEXT("GameObject already has a Transform Component"));
    Transform* ts = new Transform(this);
    mTransform = ts;
    ts->mGameObject = this;

    ts->awake();
    ts->onEnable();
}


void GameObject::addTransform(Transform* parent)
{
    ASSERT(mTransform == nullptr, TEXT("GameObject already has a Transform Component"));
    ASSERT(parent != nullptr, TEXT("parent is nullptr"));
    Transform* ts = new Transform(this, parent);
    mTransform = ts;
    ts->mGameObject = this;

    ts->awake();
    ts->onEnable();
}

void GameObject::addRectTransform(Transform* parent, Canvas* canvas)
{
    ASSERT(mTransform == nullptr, TEXT("GameObject already has a Transform Component"));
    ASSERT(parent != nullptr, TEXT("parent is nullptr"));
    ASSERT(canvas != nullptr,TEXT("canvas is nullptr"))
    RectTransform* ts = new RectTransform(this, parent, canvas);
    mTransform = ts;
    ts->mGameObject = this;

    ts->awake();
    ts->onEnable();
}

Component* GameObject::addComponent(const std::string& componentName)
{
    ASSERT(componentName != "Transform" && componentName != "RectTransform",
        TEXT("Please Use GameObject Instance Method <addTransform> to attach Transform Component"))

    ASSERT(mComponents.find(componentName) == mComponents.end(),
        TEXT("Component has already existed in this GameObject"))
    
    Component* cm = ComponentFactory::sCreateComponent(componentName);
    ASSERT(cm != nullptr, TEXT("ComponentFactory return a nullptr"))

    //bind go
    cm->mGameObject = this;
    
    mComponents[componentName] = cm;

    //we need call awake and onEnable after bind go
    cm->awake();
    cm->onEnable();

    return cm;
}

void GameObject::removeComponent(const std::string& componentName)
{
    auto iter = mComponents.find(componentName);
    ASSERT(iter != mComponents.end(), TEXT("Component does not exist in this GameObject"))
    
    ComponentFactory::sDestroyComponent(iter->second);
    
    mComponents.erase(iter);
}

Component* GameObject::getComponent(const std::string& componentName)
{
    if (componentName == "Transform")
        return mTransform;
    auto iter = mComponents.find(componentName);
    if (iter == mComponents.end())
        return nullptr;
    return iter->second;
}

void GameObject::setLayer(const Layer layer)
{
    mLayer = static_cast<unsigned char>(layer);
}

unsigned char GameObject::getLayer() const
{
    return mLayer;
}

void GameObject::activeGameObject()
{
    if (mActive)
        return;
    
    mTransform->foreachPreorder(
        [](const Transform* thisTransform)
        {
            thisTransform->mGameObject->setActive(true);
        }
        );
}

void GameObject::deactiveGameObject()
{
    if (!mActive)
        return;
    mTransform->foreachPreorder(
        [](const Transform* thisTransform)
        {
            thisTransform->mGameObject->setActive(false);
        }
        );
}

bool GameObject::isActive() const
{
    return mActive;
}

void GameObject::printSelf() const
{
    std::cout << "<GameObject Name>: " << mName << " ";
    std::cout << "<GameObject Layer>: " << static_cast<int>(mLayer) << " ";
    std::cout << "<GameObject Active>: " << mActive << " ";

    std::cout << "<GameObject Components>: ";
    for (auto& cm: mComponents)
    {
        std::cout << cm.first << " ";
    }
    std::cout<<std::endl;
}

TpUnorderedSet<GameObject*> GameObjectFactory::sGameObjects;
TpList<GameObject*> GameObjectFactory::sGarbageList;
TpList<std::function<void(GameObject* thisGo)>> GameObjectFactory::DestroyCallBackList;
TpList<std::function<void(GameObject* thisGo)>> GameObjectFactory::CreateCallBackList;

GameObject* GameObjectFactory::sCreateGameObject(const TpString& name)
{
    GameObject* go = new GameObject(name);
    sGameObjects.insert(go);

    //call back
    for (auto& func: DestroyCallBackList)
    {
        func(go);
    }
        
    return go;
}

void GameObjectFactory::sDestroyGameObject(GameObject*& go)
{
    if (go->isWillDestroy)
        return;
    //remove self from unordered_set
    auto itor = GameObjectFactory::sGameObjects.find(go);

    go->isWillDestroy = true;

    //ASSERT(itor != GameObjectFactory::sGameObjects.end(), TEXT("sGameObjects do not have this gameobject!"))
    //avoid repetitive destroy
    if (itor != GameObjectFactory::sGameObjects.end())
    {
        GameObjectFactory::sGameObjects.erase(go);
    }
    
    //destroy all components firstly
    for (auto& cm: go->mComponents)
    {
        ComponentFactory::sDestroyComponent(cm.second);
    }

    //destroy its child, using post-order
    if (go->mTransform != nullptr)
    {
        //do not need erase child from children, transform will unbind father
        for (auto& child: go->mTransform->getChildren())
        {
            GameObject* go = child->getGameObject();
            sDestroyGameObject( go);
        }
        //the transform will push in garbage ordered by post-order, and unbind father safely
        ComponentFactory::sDestroyComponent(go->mTransform);
    }
    sGarbageList.push_back(go);

    //call back
    for (auto& func: DestroyCallBackList)
    {
        func(go);
    }
}

void GameObjectFactory::sGarbageCollect()
{
    for (auto& go:sGarbageList)
    {
        SAFE_DELETE_POINTER(go);
    }
}

void GameObjectFactory::sDestroyScene()
{
    //Destroy all gameobject except the root
    Transform* root = Transform::sGetRoot();
    for(auto& ts:root->mChildren)
    {
        sDestroyGameObject(ts->mGameObject);
    }
}



