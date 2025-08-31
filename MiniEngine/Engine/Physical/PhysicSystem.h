#pragma once
#include "Engine/Component/Physics/RigidBody.h"

struct CollisionInfo {
    Vector3 normal;
    float penetrationDepth;
};

struct CollisionPair {
    RigidBody* sourceBody;
    RigidBody* targetBody;
};


// 射线检测
struct RaycastHit {
    RigidBody* body;
    Vector3 point;
    Vector3 normal;
    float distance;
};

typedef std::function<void(RigidBody*, RigidBody*)> CollisionCallback;

class PhysicSystem
{
public:
    static PhysicSystem& getInstance();

    void update(float fixedDeltaTime);

    // 添加||删除刚体到系统中
    void addRigidBody(RigidBody* body);
    void removeRigidBody(RigidBody* body);
    
    // 触发回调
    void triggerCollisionCallbacks();
    
    bool raycast(const Vector3& origin, const Vector3& direction, float maxDistance, RaycastHit& hit) const;
    
private:
    const Vector3 Gravity = Vector3(0, -9.8f, 0);
    const float GroundFriction = 2.5f;
    std::vector<RigidBody*> rigidBodies;
    std::vector<CollisionPair> collisionPairs;
    // 更新刚体逻辑和碰撞逻辑
    void updateRigidBodies(float fixedDeltaTime) const;
    void collisionUpdate();

    // 处理碰撞逻辑
    static void resolveCollision(RigidBody* a, RigidBody* b);
    // 处理碰撞信息
    static CollisionInfo calculateCollisionInfo(RigidBody* a, RigidBody* b, ShapeType typeA, ShapeType typeB,
                                                const Vector3& aPos,
                                                const Vector3& bPos);
    // 处理冲量
    static void applyImpulse(RigidBody* a, RigidBody* b, const Vector3& normal);
    // 修正位置
    static void applyPositionCorrection(RigidBody* a, RigidBody* b, const Vector3& normal, float penetrationDepth);
    
};


