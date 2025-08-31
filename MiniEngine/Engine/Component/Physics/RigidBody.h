#pragma once

#include "Engine/math/math.h"
#include "Engine/Physical/Shape/BaseShape.h"
#include "Engine/Component/Component.h"
#include "Engine/Component/Transform.h"
#include "Engine/Render/Material.h"
#include "Engine/Scene/ISerializable.h"

class RigidBody : public Component {
public:
    RigidBody();
    ~RigidBody();

    void awake() override;

    float getMass() const;
    float getInvMass() const;
    // 如果Mass设成0，物体静止
    void setMass(float newMass);
    
    template<typename T>
    T* getShape() const {
        return dynamic_cast<T*>(mShape);
    }
    void setShape(BaseShape* newShape);

    Vector3 getPosition() const;

    Vector3 getVelocity() const;
    void applyVelocity(Vector3 newVelocity);
    void setVelocity(const Vector3& newVelocity);

    Vector3 getForce() const;
    void applyForce(const Vector3& newForce);
    void setForce(const Vector3& newForce);
    // void update() override;

    ShapeType getShapeType() const;
    void getAABB(Vector3& min, Vector3& max) const;
    bool localIsInside(const Vector3& point) const;

    Transform* getTransform() const;

    void isGravity(bool newIsGravity);
    bool getIsGravity() const;

    void addCollisonCallback(const std::function<void(RigidBody*, RigidBody*)>& newCallback);
    void clearCollisionCallback();
    std::list<std::function<void(RigidBody*, RigidBody*)>>* getCollisionCallbackList(){return &onCollisionList;}

    void prepareRenderList() const;
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value);
    void deSerialize(const rapidxml::xml_node<>* node);
    void showSelf() override;

private:
    std::list<std::function<void(RigidBody*, RigidBody*)>> onCollisionList;
    float mass;
    float invMass;  // 质量的倒数
    Vector3 velocity;  // 速度
    Vector3 force ;     // 力
    float volume;      // 体积
    bool useGravity = false; // 重力开关

    BaseShape* mShape = nullptr;
    std::unique_ptr<MaterialInstance> mMaterialGpu = nullptr;
};
