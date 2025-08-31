#include "PhysicSystem.h"

#include "Shape/BoxShape.h"
#include <algorithm>

#include "Engine/Component/GameObject.h"
#include "Shape/SphereShape.h"

PhysicSystem& PhysicSystem::getInstance()
{
    static PhysicSystem instance;
    return instance;
}

void PhysicSystem::update(float fixedDeltaTime)
{
    updateRigidBodies(fixedDeltaTime);
    collisionUpdate();
    triggerCollisionCallbacks();
}

void PhysicSystem::updateRigidBodies(float fixedDeltaTime) const
{
    for (RigidBody* rb : rigidBodies)
    {
        if (rb->getInvMass() <= 0.0f) continue; // 忽略静态物体

        if (rb->getGameObject()->getName() == "shell")
        {
            std::cout<<"111";
        }
        
        Transform* transform = rb->getTransform();
        
        // 应用重力
        if (rb->getIsGravity()) {
            rb->applyForce(Gravity * rb->getMass());
        }
        
        // 计算加速度 a = F/m
        float invMass = rb->getInvMass();
        Vector3 acceleration = rb->getForce() * invMass;
        
        // 更新 v = v + a*t
        rb->applyVelocity(acceleration * fixedDeltaTime);

        // 计算摩擦力
        Vector3 midVelocity = rb->getVelocity();
        
        // 地面摩擦
        Vector3 frictVelocity(midVelocity.v.x, 0.0f, midVelocity.v.z);
        float frictSpeed = frictVelocity.Length();
        
        if (frictSpeed > 0.01f) 
        {
            // 计算地面摩擦减速量
            float frictDeceler = GroundFriction * fixedDeltaTime;
            
            if (frictSpeed <= frictDeceler)
            {
                midVelocity.v.x = 0.0f;
                midVelocity.v.z = 0.0f;
            }
            else
            {
                // 摩擦力的方向
                Vector3 frictionDir = frictVelocity.Normalize() * -1.0f;
                
                // 应用摩擦力
                midVelocity.v.x += frictionDir.v.x * frictDeceler;
                midVelocity.v.z += frictionDir.v.z * frictDeceler;
            }
        }
        
        rb->setVelocity(midVelocity);
        
        // 更新 x = x + v*t
        if (transform)
        {
            Vector3 newPosition = transform->getWorldPosition() + (rb->getVelocity() * fixedDeltaTime);
            transform->setWorldPosition(newPosition);
        }
        // 更新位置
        if (rb->getShape<BaseShape>())
        {
            rb->getShape<BaseShape>()->setPosition(transform->getWorldPosition());
        }
        
        // 重置力
        rb->setForce(kZeroVector3);
    }
}

void PhysicSystem::collisionUpdate()
{
    collisionPairs.clear();
    for (size_t i = 0; i < rigidBodies.size(); ++i) {
        RigidBody* a = rigidBodies[i];
        if (!a->getGameObject()->isActive())
            continue; //不激活物体不参与碰撞检测
        for (size_t j = i + 1; j < rigidBodies.size(); ++j) {
            RigidBody* b = rigidBodies[j];
            
            if (!b->getGameObject()->isActive())
                continue; //不激活物体不参与碰撞检测

            if (a->getInvMass() <= 0.0f && b->getInvMass() <= 0.0f) continue;

            if (a->getShape<BaseShape>() && b->getShape<BaseShape>()) {
                // 使用AABB进行宽相位碰撞检测
                Vector3 aMin, aMax, bMin, bMax;
                a->getShape<BaseShape>()->getAABB(a->getTransform()->getWorldPosition(), aMin, aMax);
                b->getShape<BaseShape>()->getAABB(b->getTransform()->getWorldPosition(), bMin, bMax);

                if (aMax.v.x < bMin.v.x || aMin.v.x > bMax.v.x) continue;
                if (aMax.v.y < bMin.v.y || aMin.v.y > bMax.v.y) continue;
                if (aMax.v.z < bMin.v.z || aMin.v.z > bMax.v.z) continue;

                // 进行窄相位碰撞检测
                if (a->getShape<BaseShape>()->checkCollision(*b->getShape<BaseShape>())) {
                    // 处理碰撞
                    resolveCollision(a, b);
                    // 碰撞对
                    collisionPairs.push_back(CollisionPair{a, b});
                }
            }
        }
    }
}

void PhysicSystem::addRigidBody(RigidBody* body)
{
    if (!body) return;
    
    auto it = std::find(rigidBodies.begin(), rigidBodies.end(), body);
    if (it == rigidBodies.end()) {
        rigidBodies.push_back(body);
        
        if (body->getShape<BaseShape>() && body->getTransform()) {
            Vector3 curPosition = body->getTransform()->getWorldPosition();
            body->getShape<BaseShape>()->setPosition(curPosition);
        }
    }
}

void PhysicSystem::removeRigidBody(RigidBody* body)
{
    auto it = std::find(rigidBodies.begin(), rigidBodies.end(), body);
    if (it != rigidBodies.end())
    {
        rigidBodies.erase(it);
    }
}


void PhysicSystem::triggerCollisionCallbacks()
{
    for (const CollisionPair& pair : collisionPairs)
    {
        // 触发sourceBody的回调
        auto callbackList = pair.sourceBody->getCollisionCallbackList();
        for (auto& callback : *callbackList)
        {
            callback(pair.sourceBody, pair.targetBody);
        }
        
        // 触发targetBody的回调
        callbackList = pair.targetBody->getCollisionCallbackList();
        for (auto& callback : *callbackList)
        {
            callback(pair.targetBody, pair.sourceBody);
        }
    }
}

bool PhysicSystem::raycast(const Vector3& origin, const Vector3& direction, float maxDistance, RaycastHit& hit) const
{
    bool hitSomething = false;
    float closestDistance = maxDistance;

    for (RigidBody* rb : rigidBodies)
        {
        if (!rb->getShape<BaseShape>()) continue;

        float t = 0; 
        Vector3 normal;

        if (rb->getShape<BaseShape>()->rayIntersect(origin, direction, maxDistance, t, normal))
        {
            if (t < closestDistance && t >= 0)
            {
                closestDistance = t;
                hit.body = rb;
                hit.point = origin + direction * t;
                hit.normal = normal;
                hit.distance = t;
                hitSomething = true;
            }
        }
    }

    return hitSomething;
}

// 处理碰撞
void PhysicSystem::resolveCollision(RigidBody* a, RigidBody* b)
{
    if ((a->getMass() < 0.01 && a->getMass() > 0) || (b->getMass() < 0.01 && b->getMass() > 0))
    {//如果质量过小，则不处理碰撞的物理修正
        return;
    }
    
    // 获取位置和速度信息
    Vector3 relVelocity = b->getVelocity() - a->getVelocity();
    Vector3 aPosition = a->getTransform()->getWorldPosition();
    Vector3 bPosition = b->getTransform()->getWorldPosition();
    
    // 检测碰撞形状类型
    ShapeType typeA = a->getShape<BaseShape>()->getType();
    ShapeType typeB = b->getShape<BaseShape>()->getType();

    // 计算碰撞法线和穿透深度
    CollisionInfo collision = calculateCollisionInfo(a, b, typeA, typeB, aPosition, bPosition);
    
    // 没有碰撞则返回
    if (collision.penetrationDepth <= 0) return;
    
    // 分离中的物体不处理
    if (Vector3::Dot(relVelocity, collision.normal) > 0.5f) return;

    // 处理速度冲量
    applyImpulse(a, b, collision.normal);

    // 处理位置修正
    applyPositionCorrection(a, b, collision.normal, collision.penetrationDepth);
}

// 计算碰撞信息
CollisionInfo PhysicSystem::calculateCollisionInfo(RigidBody* a, RigidBody* b, 
                                                  ShapeType typeA, ShapeType typeB,
                                                  const Vector3& aPos, const Vector3& bPos)
{
    CollisionInfo info;
    
    if (typeA == ShapeType::Box && typeB == ShapeType::Box)
        {
        // Box-Box 碰撞处理
        Vector3 aMin, aMax, bMin, bMax;
        a->getShape<BoxShape>()->getAABB(aPos, aMin, aMax);
        b->getShape<BoxShape>()->getAABB(bPos, bMin, bMax);

        Vector3 overlap(
            std::min(aMax.v.x, bMax.v.x) - std::max(aMin.v.x, bMin.v.x),
            std::min(aMax.v.y, bMax.v.y) - std::max(aMin.v.y, bMin.v.y),
            std::min(aMax.v.z, bMax.v.z) - std::max(aMin.v.z, bMin.v.z)
        );

        info.penetrationDepth = std::min(overlap.v.x, std::min(overlap.v.y, overlap.v.z));
        
        // 选择最小重叠轴作为法线方向
        if (info.penetrationDepth == overlap.v.x)
        {
            // 使用物体中心来确定法线方向
            Vector3 centerA = (aMin + aMax) * 0.5f;
            Vector3 centerB = (bMin + bMax) * 0.5f;
            info.normal = (centerB.v.x > centerA.v.x) ? Vector3(1, 0, 0) : Vector3(-1, 0, 0);
        } else if (info.penetrationDepth == overlap.v.y) {
            Vector3 centerA = (aMin + aMax) * 0.5f;
            Vector3 centerB = (bMin + bMax) * 0.5f;
            info.normal = (centerB.v.y > centerA.v.y) ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
        } else {
            Vector3 centerA = (aMin + aMax) * 0.5f;
            Vector3 centerB = (bMin + bMax) * 0.5f;
            info.normal = (centerB.v.z > centerA.v.z) ? Vector3(0, 0, 1) : Vector3(0, 0, -1);
        }

    } else if (typeA == ShapeType::Sphere && typeB == ShapeType::Sphere) {
        // Sphere-Sphere 碰撞处理
        Vector3 dir = bPos - aPos;
        float distance = dir.Length();
        float radiusSum = a->getShape<SphereShape>()->getRadius() + 
                         b->getShape<SphereShape>()->getRadius();
        
        info.penetrationDepth = radiusSum - distance;
        info.normal = (distance > 0) ? dir / distance : Vector3(1, 0, 0);

    } else {
        // Sphere-Box 碰撞处理
        RigidBody* sphere = (typeA == ShapeType::Sphere) ? a : b;
        RigidBody* box = (typeA == ShapeType::Box) ? a : b;
        
        Vector3 spherePos = (typeA == ShapeType::Sphere) ? aPos : bPos;
        Vector3 boxMin, boxMax;
        box->getShape<BoxShape>()->getAABB((typeA == ShapeType::Box) ? aPos : bPos, boxMin, boxMax);

        Vector3 closestPoint = Vector3(
            (spherePos.v.x < boxMin.v.x) ? boxMin.v.x : (spherePos.v.x > boxMax.v.x) ? boxMax.v.x : spherePos.v.x,
            (spherePos.v.y < boxMin.v.y) ? boxMin.v.y : (spherePos.v.y > boxMax.v.y) ? boxMax.v.y : spherePos.v.y,
            (spherePos.v.z < boxMin.v.z) ? boxMin.v.z : (spherePos.v.z > boxMax.v.z) ? boxMax.v.z : spherePos.v.z
        );
        
        Vector3 dir = spherePos - closestPoint;
        float distance = dir.Length();
        info.penetrationDepth = sphere->getShape<SphereShape>()->getRadius() - distance;
        info.normal = (distance > 0) ? dir / distance : Vector3(1, 0, 0);
    }

    return info;
}

// 应用冲量
void PhysicSystem::applyImpulse(RigidBody* a, RigidBody* b, const Vector3& normal) {
    Vector3 relVelocity = b->getVelocity() - a->getVelocity();
    float velAlongNormal = Vector3::Dot(relVelocity, normal);
    
    if (fabs(velAlongNormal) < 0.01f) return; // 忽略极小速度
    float restitution = 0.2f; // 弹性系数
    float invMassA = a->getInvMass();
    float invMassB = b->getInvMass();
    float j = -(1 + restitution) * velAlongNormal;
    j /= invMassA + invMassB;

    Vector3 impulse = normal * j;
    if (invMassA > 0) {
        a->setVelocity(a->getVelocity() - impulse * invMassA);
    }
    if (invMassB > 0) {
        b->setVelocity(b->getVelocity() + impulse * invMassB);
    }
}

// 应用位置修正
void PhysicSystem::applyPositionCorrection(RigidBody* a, RigidBody* b, 
                                          const Vector3& normal, float penetrationDepth) {
    if (penetrationDepth <= 0.05f) return;
    
    const float kSlop = 0.005f; // 穿透容差
    const float kCorrection = 0.06f; // 降低修正系数
    
    float invMassA = a->getInvMass();
    float invMassB = b->getInvMass();
    float invMassSum = invMassA + invMassB;
    
    // 都是静态物体，则不进行修正
    if (invMassSum <= 0.0f) return;
    
    // 计算修正量
    Vector3 correction = normal * (std::max(0.0f, penetrationDepth - kSlop) * kCorrection / invMassSum);
    
    // 应用修正
    if (invMassA > 0) {
        a->getTransform()->setWorldPosition(
            a->getTransform()->getWorldPosition() - correction * invMassA);
    }
    if (invMassB > 0) {
        b->getTransform()->setWorldPosition(
            b->getTransform()->getWorldPosition() + correction * invMassB);
    }
    
    // 更新形状位置
    a->getShape<BaseShape>()->setPosition(a->getTransform()->getWorldPosition());
    b->getShape<BaseShape>()->setPosition(b->getTransform()->getWorldPosition());
}