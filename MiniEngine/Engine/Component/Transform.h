#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Engine/math/math.h"
#include "Component.h"
#include "Engine/Scene/ISerializable.h"

///reference unity Transform implementation
///only store an object local transform information
///then define how to get world transform information
///when we tranform a gameobject, we only need set itself transform, no need to change its children
class Transform: public Component 
{
    friend class ComponentFactory;
    friend class GameObject;
    friend class GameObjectFactory;
public:
    //virtual void awake() override;
    //virtual void onEnable() override;
    virtual void start() override;
    virtual void update() override;
    //virtual void onDisable() override;
    //virtual void onDestory() override;
    
    //TreeNode Part
    int32_t getChildIndex(const Transform* child) const;
    size_t getChildCount() const {return mChildren.size();}
    void downSelfToLastChild();
    void upSelfToFirstChild();
    Transform* getChild(const size_t index) const;
    Transform* getChild(const std::string& name) const;
    std::vector<Transform*> getChildren() const;
    void getChildrenWithName(const std::string& name, std::vector<Transform*>& result) const;
    Transform* getChildWithName(const std::string& name);
    static Transform* sFindGameObject(const std::string& name); //Golbal Search

    static Transform* sGetRoot() {return &sTree;}

    void setParent(Transform* parent);
    Transform* getParent() const {return mParent;}
    
    void addChildren(GameObject* go);
    void addChildren(Transform* ts);
    
    void removeChild(const int32_t index, Transform* newParent);
    
    void swapChildIndex(const Transform* child1, const Transform* child2); //switch child position in Tree

    //iterate all node in this tree
    void foreachPreorder(const std::function<void(const Transform* transform)>& func) const;
    void foreachActivePreorder(const std::function<void(const Transform* transform)>& func) const;
    void foreachActiveLevelOrder(const std::function<void(const Transform* transform)>& func);

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //Position Part, this should return data copy
    virtual Vector3 getWorldPosition(const Vector2& offset = {0,0}) const;
    Vector3 getLocalPosition() const {return mLocalPosition;}
    
    virtual Quaternion getWorldRotation() const;
    Quaternion getLocalRotation() const {return mLocalRotation;}
    
    
    virtual Vector3 getWorldScale() const;
    Vector3 getLocalScale() const {return mLocalScale;}

    ///get World Forward
    Vector3 getForward() const;
    ///get World Up
    Vector3 getUp() const;
    ///get World Right
    Vector3 getRight() const;
    
    //Transform Point
    Vector3 TransformPointToWorld(const Vector3& vec) const;
    Vector3 TransformPointToLocal(const Vector3& vec) const;
    Vector3 TransformDirectionToWorld(const Vector3& vec) const;
    Vector3 TransformDirectionToLocal(const Vector3& vec) const;

    virtual Matrix4x4 getModelMatrix() const;
    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    
    ///set
    void setWorldPosition(const Vector3& newPosition);
    void setLocalPosition(const Vector3& newPosition) {mLocalPosition = newPosition;}

    void setWorldRotation(const Quaternion& newRotation);
    void setLocalRotation(const Quaternion& newRotation) {mLocalRotation = newRotation;}
    
    void rotateLocalPitchYawRoll(const Vector3& rotation);
    void rotateAroundWorldAxis(const Vector3& axis, const float angle);
    void rotateAroundLocalAxis(const Vector3& axis, const float angle);

    void setLocalScale(const Vector3& newScale) {mLocalScale = newScale;};

    ///Directly move this transform local position
    void movePosition(const Vector3& displacement) {mLocalPosition += displacement;}
    void movePosition(const Vector3& direction, float distance) {mLocalPosition += (direction * distance);}

    //rotate this transform to look at world destination pos (forward direction)
    void lookAtWorldPosition(const Vector3& worldDestinationPos);
    //rotate this transform to look at world direction (forward direction)
    void lookAtWorldDirection(const Vector3& worldDirection);
    
    //debug
    void printAll(uint32_t layer=0) const;
    void printSelf() const;

    //serialization
    virtual rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "") override;
    virtual void deSerialize(const rapidxml::xml_node<>* node) override;

    void showSelf() override;
    
protected:
    DELETE_CONSTRUCTOR_FIVE(Transform);
    Transform();
    virtual ~Transform() override;
    Transform(GameObject* go,
        Vector3 newLocalPosition = Vector3(0,0,0),
        Vector3 newLocalRotation = Vector3(0,0,0),
        Vector3 newLocalScale = Vector3(1,1,1));
    Transform(GameObject* go, Transform* parent,
        Vector3 newLocalPosition = Vector3(0,0,0),
        Vector3 newLocalRotation = Vector3(0,0,0),
        Vector3 newLocalScale = Vector3(1,1,1));

    //Tree
    static Transform sTree; //static tree, mange the whole go tree
    
    void unbindFather();
    std::vector<Transform*> mChildren;
    Transform* mParent = nullptr;

    //Transform only save local information in its father's coordinate system
    Vector3 mLocalPosition;
    Quaternion mLocalRotation;
    Vector3 mLocalScale{1.0f,1.0f,1.0f};
};
