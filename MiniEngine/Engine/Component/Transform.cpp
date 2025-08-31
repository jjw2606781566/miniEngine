#include <queue>
#include <cmath>
#include <iostream>

#include "Transform.h"
#include "GameObject.h"
#include "Engine/common/Exception.h"
#include "TGUI/RectTransform.h"

Transform Transform::sTree;

///this default constructor is used to create the root of the tree
Transform::Transform()
{ 
    GameObject* go = new GameObject("Root");
    mGameObject = go;
    mGameObject->setLayer(Layer::LAYER_NONE);
    go->mTransform = this;
}

//the default father is sTree
Transform::Transform(GameObject* go, Vector3 newLocalPosition, Vector3 newLocalRotation, Vector3 newLocalScale)
    :mLocalPosition(newLocalPosition), mLocalRotation(), mLocalScale(newLocalScale)
{
    rotateLocalPitchYawRoll(newLocalPosition);
    sTree.addChildren(this);
}

Transform::Transform(GameObject* go, Transform* parent, Vector3 newLocalPosition, Vector3 newLocalRotation, Vector3 newLocalScale)
    :mLocalPosition(newLocalPosition), mLocalRotation(), mLocalScale(newLocalScale)
{
    rotateLocalPitchYawRoll(newLocalPosition);
    parent->addChildren(this);
}

//Transform do not control any life cycle of its children,but need unbind its father
Transform::~Transform()
{
    unbindFather();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Transform::start()
{
    DEBUG_PRINT("<%s> Transform Component Start()\n", mGameObject->getName().c_str());
}

void Transform::update()
{
    //DEBUG_PRINT("<%s> Transform Component Update()\n", mGameObject->getName().c_str());
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int32_t Transform::getChildIndex(const Transform* child) const
{
    ASSERT(child != nullptr, TEXT("child is nullptr"));
    for (size_t index = 0; index < mChildren.size(); ++index)
    {
        if (mChildren[index] == child)
        {
            return static_cast<int32_t>(index);
        }
    }
    return -1;
}

void Transform::downSelfToLastChild()
{
    auto itor = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
    ASSERT(itor != mParent->mChildren.end() ,TEXT("Transform can not find child!"))
    mParent->mChildren.erase(itor);
    mParent->mChildren.push_back(this);
}

void Transform::upSelfToFirstChild()
{
    auto itor = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
    ASSERT(itor != mParent->mChildren.end() ,TEXT("Transform can not find child!"))
    mParent->mChildren.erase(itor);
    mParent->mChildren.insert(mParent->mChildren.begin(),this);
}

Transform* Transform::getChild(const size_t index) const
{
    ASSERT(index < mChildren.size(), TEXT("index is out of range"));
    return mChildren[index];
}

Transform* Transform::getChild(const std::string& name) const
{
    //use BFS to find the child
    std::queue<Transform*> que;
    for (auto& child : mChildren)
    {
        if (child->getGameObjectName() == name)
            return child;
        else
        {
            que.push(child);
        }
    }
    while (!que.empty())
    {
        Transform* tf = que.front();
        que.pop();
        for (auto& child : tf->mChildren)
        {
            if (child->getGameObjectName() == name)
                return child;
            else
            {
                que.push(child);
            }
        }
    }
    
    return nullptr;
}

void Transform::getChildrenWithName(const std::string& name, std::vector<Transform*>& result) const
{
    result.clear();
    //use BFS to find children
    std::queue<Transform*> que;
    for (auto& child : mChildren)
    {
        if (child->getGameObjectName() == name)
            result.push_back(child);
        else
        {
            que.push(child);
        }
    }
    while (!que.empty())
    {
        Transform* tf = que.front();
        que.pop();
        for (auto& child : tf->mChildren)
        {
            if (child->getGameObjectName() == name)
                result.push_back(child);
            else
            {
                que.push(child);
            }
        }
    }
}

Transform* Transform::getChildWithName(const std::string& name)
{
    //use BFS to find children
    std::queue<Transform*> que;
    for (auto& child : mChildren)
    {
        if (child->getGameObjectName() == name)
            return child;
        else
        {
            que.push(child);
        }
    }
    while (!que.empty())
    {
        Transform* tf = que.front();
        que.pop();
        for (auto& child : tf->mChildren)
        {
            if (child->getGameObjectName() == name)
                return child;
            else
            {
                que.push(child);
            }
        }
    }
    return nullptr;
}


///Golbal Search go, this method is slow
Transform* Transform::sFindGameObject(const std::string& name)
{
    for (auto& child: sTree.mChildren)
    {
        if (child->getGameObjectName() == name)
            return child;
        else
        {
            Transform* tf = child->getChild(name);
            if (tf != nullptr)
                return tf;
        }
    }
    return nullptr;
}

std::vector<Transform*> Transform::getChildren() const
{
    return mChildren;
}

///unbind its father
void Transform::unbindFather()
{
    if (mParent != nullptr)
    {
        int32_t index = mParent->getChildIndex(this);

        //unbind its father
        ASSERT(index != -1, TEXT("ts is not the child of it's parent"));
        mParent->mChildren.erase(mParent->mChildren.begin() + index);

        mParent = nullptr;
    }
}

void Transform::setParent(Transform* parent)
{
    ASSERT(parent != nullptr, TEXT("parent is nullptr"));
    parent->addChildren(this);
}

void Transform::addChildren(GameObject* go)
{
    ASSERT(go != nullptr, TEXT("child is nullptr"));
    Transform* ts = go->getTransform();

    //unbind its original father
    ts->unbindFather();

    //set new father
    mChildren.push_back(ts);
    ts->mParent = this;
}

void Transform::addChildren(Transform* ts)
{
    ASSERT(ts != nullptr, TEXT("child is nullptr"));

    //unbind its original father
    ts->unbindFather();

    //set new father
    mChildren.push_back(ts);
    ts->mParent = this;
}

void Transform::removeChild(const int32_t index, Transform* newParent)
{
    Transform* child = getChild(index);
    ASSERT(child != nullptr, TEXT("getChild index return nullptr"));

    //don't need to unbind father
    newParent->addChildren(child);
}


void Transform::swapChildIndex(const Transform* child1, const Transform* child2)
{
    int32_t index1 = getChildIndex(child1);
    int32_t index2 = getChildIndex(child2);
    ASSERT((index1 != -1 && index2 != -1), TEXT("child1 or child2 is not the child of this transform"));
    std::swap(mChildren[index1], mChildren[index2]);
}

void Transform::foreachPreorder(const std::function<void(const Transform* thisTransform)>& func) const
{
    //iterate all children, using DFS-Preorder
    ASSERT(this != nullptr, TEXT("Transform this pointer is nullptr"));
    func(this);
    //do not use iterator! Expansion will cause iterators to fail
    /*for (auto& child : mChildren)
    {
        child->foreachPreorder(func);
    }*/
    for (size_t i = 0; i<mChildren.size(); ++i)
    {
        mChildren[i]->foreachPreorder(func);
    }
}

void Transform::foreachActivePreorder(const std::function<void(const Transform* transform)>& func) const
{
    //iterate all active children, using DFS-Preorder
    ASSERT(this != nullptr, TEXT("Transform this pointer is nullptr"));
    if (! mGameObject->isActive())
        return;
    func(this);
    //do not use iterator! Expansion will cause iterators to fail
    /*for (auto& child : mChildren)
    {
        child->foreachActivePreorder(func);
    }*/
    for (size_t i = 0; i<mChildren.size(); ++i)
    {
        mChildren[i]->foreachActivePreorder(func);
    }
}

void Transform::foreachActiveLevelOrder(const std::function<void(const Transform* transform)>& func)
{
    ASSERT(this != nullptr, TEXT("Transform this pointer is nullptr"));
    std::queue<Transform*> que;
    if (! mGameObject->isActive())
        return;
    que.push(this);
    static Transform* transform = nullptr;
    while (!que.empty())
    {
        transform = que.front();
        que.pop();
        func(transform);
        for (size_t i = 0; i<transform->mChildren.size(); ++i)
        {
            if (! transform->getGameObject()->isActive())
            {
                continue;
            }
            que.push(transform->mChildren[i]);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3 Transform::getWorldPosition(const Vector2& offset) const
{
    Vector3 worldPosition = mLocalPosition;
    Transform* father = mParent;
    while (father != nullptr)
    {
        worldPosition.Scale(father->mLocalScale);
        father->mLocalRotation.QuaternionRotateVector(worldPosition);
        worldPosition += father->mLocalPosition;

        father = father->mParent;
    }
    return worldPosition;
}

Quaternion Transform::getWorldRotation() const
{
    Quaternion worldRotation = mLocalRotation;
    Transform* father = mParent;
    while (father != nullptr)
    {
        worldRotation *= father->mLocalRotation;
        
        father = father->mParent;
    }
    worldRotation.normalize();
    return worldRotation;
}

Vector3 Transform::getWorldScale() const
{
    Vector3 worldScale = mLocalScale;
    Transform* father = mParent;
    while (father != nullptr)
    {
        worldScale.Scale(father->mLocalScale);
        
        father = father->mParent;
    }
    return worldScale;
}

Vector3 Transform::getForward() const
{
    return TransformDirectionToWorld({0,0,1}).Normalize();
}

Vector3 Transform::getUp() const
{
    return TransformDirectionToWorld({0,1,0}).Normalize();
}

Vector3 Transform::getRight() const
{
    return TransformDirectionToWorld({1,0,0}).Normalize();
}

///this function is transform a point in !!!this!!! coordinate to world coordinate
Vector3 Transform::TransformPointToWorld(const Vector3& vec) const
{
    Vector3 worldPosition = vec;
    const Transform* father = this;
    while (father != nullptr)
    {
        worldPosition.Scale(father->mLocalScale);
        father->mLocalRotation.QuaternionRotateVector(worldPosition);
        worldPosition += father->mLocalPosition;

        father = father->mParent;
    }
    return worldPosition;
}

///this function is transform a point in world coordinate to !!!this!!! coordinate
Vector3 Transform::TransformPointToLocal(const Vector3& vec) const
{
    Vector3 localPosition;
    Transform* father = mParent;
    if (father != nullptr)
        localPosition = father->TransformPointToLocal(vec);
    else
        localPosition = vec;

    localPosition -= mLocalPosition;
    Quaternion InvQ = mLocalRotation.getInverse();
    InvQ.QuaternionRotateVector(localPosition);
    localPosition.Scale(Vector3::InverseSafe(mLocalScale));

    return localPosition;
}

Vector3 Transform::TransformDirectionToWorld(const Vector3& vec) const
{
    //faster than transform point because do not need scale and move
    Quaternion rotation = getWorldRotation();
    Vector3 result = vec;
    rotation.QuaternionRotateVector(result);
    return result;
}

Vector3 Transform::TransformDirectionToLocal(const Vector3& vec) const
{
    Quaternion rotation = getWorldRotation().getInverse();
    Vector3 result = vec;
    rotation.QuaternionRotateVector(result);
    return result;
}

Matrix4x4 Transform::getModelMatrix() const
{
    Matrix4x4 model;
    auto temp1 = getWorldPosition();
    auto temp2 = getWorldRotation();
    auto temp3 = getWorldScale();
    model.setModelMatrixQuaternion(getWorldPosition(), getWorldRotation(), getWorldScale());
    return model;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Transform::setWorldPosition(const Vector3& newPosition)
{
    //we need get newWorldPosition's vector in father's coordinate
    Vector3 result;
    Transform* father = mParent;
    if (father != nullptr)
        result = father->TransformPointToLocal(newPosition);

    setLocalPosition(result);
}

void Transform::setWorldRotation(const Quaternion& newRotation)
{
    Transform* father = mParent;
    if (father != nullptr)
    {
        setLocalRotation( father->getWorldRotation().getInverse() * newRotation );
    }
    else
        setLocalRotation(newRotation);
}

void Transform::rotateLocalPitchYawRoll(const Vector3& rotation)
{
    Quaternion q;
    q.setQuaternionRotationRollPitchYaw(rotation);
    //a mesh vertex need rotate in this coordinate firstly
    mLocalRotation = q * mLocalRotation;
}

void Transform::rotateAroundWorldAxis(const Vector3& axis, const float angle)
{
    Vector3 localAxis = TransformDirectionToLocal(axis);
    Quaternion q;
    q.setToRotateAboutAxis(localAxis, angle);
    mLocalRotation = q * mLocalRotation;
    mLocalRotation.normalize();
}

void Transform::rotateAroundLocalAxis(const Vector3& axis, const float angle)
{
    Quaternion q;
    q.setToRotateAboutAxis(axis, angle);
    mLocalRotation = q * mLocalRotation;
    mLocalRotation.normalize();
}

void Transform::lookAtWorldPosition(const Vector3& destinationPos)
{
    Vector3 worldDirection = destinationPos - getWorldPosition();
    lookAtWorldDirection(worldDirection);
    
}

void Transform::lookAtWorldDirection(const Vector3& direction)
{
    Vector3 localDirection = TransformDirectionToLocal(direction).Normalize();
    Vector3 localCurrentForward{0,0,1};
    float angle = Vector3::Angle(localCurrentForward, localDirection);
    Vector3 axis = Vector3::CrossProduct(localCurrentForward, localDirection);
    
    //if the axis is zero, it means the two vector are parallel, just return
    if (axis.Length() < MathUtils::FLOAT_EPSILON)
    {
        
        return;
    }

    Quaternion q;
    q.setToRotateAboutAxis(axis, angle);

    //do not use setRotation, because all of this rotation is in this coordinate
    mLocalRotation *= q;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Transform::printAll(uint32_t layer) const
{
    //DFS serialization
    //use DFS-preorder to output children
    for (uint32_t i = 0; i < layer; ++i)
    {
        std::cout<<"\t";
    }
    printSelf();
    for (auto& child : mChildren)
    {
        child->printAll(layer + 1);
    }
}

void Transform::printSelf() const
{
    std::cout<<"{Transform}"<<" ";
    if (mGameObject != nullptr)
        std::cout<<"<goName>: "<<mGameObject->getName()<<" ";
    else
        std::cout<<"<goName>: "<<"nullptr"<<" ";
    if (mParent != nullptr)
        std::cout<<"<parent>: "<<mParent->mGameObject->getName()<<" ";
    else
        std::cout<<"<parent>: "<<"nullptr"<<" ";
    std::cout<<"<children>: ";
    for (auto& child : mChildren)
    {
        std::cout<<child->mGameObject->getName()<<" ";
    }
    std::cout<<std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
rapidxml::xml_node<>* Transform::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("Transform")));
    
    mLocalPosition.serialize(doc, mXmlNode, "LocalPosition");
    mLocalRotation.serialize(doc, mXmlNode, "LocalRotation");
    mLocalScale.serialize(doc, mXmlNode, "LocalScale");
    
    return mXmlNode;
}

void Transform::deSerialize(const rapidxml::xml_node<>* node)
{
    auto currentNode = node->first_node("Vector3");
    mLocalPosition.deSerialize(currentNode);
    currentNode = currentNode->next_sibling("Quaternion");
    mLocalRotation.deSerialize(currentNode);
    currentNode = currentNode->next_sibling("Vector3");
    mLocalScale.deSerialize(currentNode);
}

void Transform::showSelf()
{
#ifdef WIN32
    static float* currentLocalRotation;
    const static int inputWidth = 50;
    const static float step = 0.1f;
    
    Vector3 rotationEular = mLocalRotation.getEulerAnglesDegree();
    currentLocalRotation = &(rotationEular.v.x);
    
    if (ImGui::TreeNode(ComponentRegister::sGetClassName(this).c_str()))
    {
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("X", &(mLocalPosition.v.x));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalPosition.v.x += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Y", &(mLocalPosition.v.y));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalPosition.v.y += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Z", &(mLocalPosition.v.z));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalPosition.v.z += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::Text("Local Position");
        if (ImGui::InputFloat3("LocalRotation", currentLocalRotation))
        {
            Quaternion newRotation;
            if (currentLocalRotation[0] < -90)
            {
                currentLocalRotation[0] = -90;
            }
            else if (currentLocalRotation[0] > 90)
            {
                currentLocalRotation[0] = 90;
            }
            newRotation.setQuaternionRotationRollPitchYaw({currentLocalRotation[0] * MathUtils::DEG_TO_RAD, 
                currentLocalRotation[1] * MathUtils::DEG_TO_RAD, currentLocalRotation[2] * MathUtils::DEG_TO_RAD});
            setLocalRotation(newRotation);
        }
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("X##xx", &(mLocalScale.v.x));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalScale.v.x += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Y##xx", &(mLocalScale.v.y));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalScale.v.y += wheel * step;
            }
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(inputWidth);
        ImGui::InputFloat("Z##xx", &(mLocalScale.v.z));
        if (ImGui::IsItemHovered())
        {
            float wheel = ImGui::GetIO().MouseWheel;
            if (wheel != 0.0f)
            {
                mLocalScale.v.z += wheel * step;
            }
        }
        ImGui::Text("LocalScale");
        ImGui::TreePop();
    }
    
#endif
    
}
