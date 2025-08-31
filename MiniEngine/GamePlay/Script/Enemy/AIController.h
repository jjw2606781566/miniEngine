#pragma once

#include "AICounter.h"
#include "EnemyState.h"
#include "Engine/AISystem/NavigationMap.h"
#include "Engine/Component/MonoBehavior.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Component/TGUI/TextTGUI.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"

class AIController : public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void onDestory() override;

    void tankMoveTo(int targetX, int targetZ);
    
    void setStopRanger(float range);
    void setActive(bool a);
    void setPatrol(bool p);
    void addPatrolTarget(int targetX, int targetZ);
    void removePatrolTarget(int targetX, int targetZ);
    void clearPatrolTargets();
    bool isActive() const;
    void stop();
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value);
    void deSerialize(const rapidxml::xml_node<>* node);

protected:
    void setMoveSpeed(float speed);
    void setMoveForce(float force);
    void setRotationSpeed(float speed);
    void setPathFindingInterval(float interval);
    
    float rotationSpeed = 0.5f;
    float moveForce = 4.0f;
    float maxSpeed = 5.0f;
    float pathFindingInterval = 0.5f;
private:
    bool active, isMoving, isPatrolling;
    EnemyState mEnemyState;
    Vector3 targetPos;
    float squaredDisTarget = std::numeric_limits<float>::max();
    bool canHitTarget;
    float attackTimer;
    float pathFindingTimer; 
    float stopRange;
    float guardRange; // 警戒距离
    float fireRange; // 攻击距离
    float maxHealth;
    int patrolPointIndex;
    std::pair<int, int> destination;
    std::vector<std::pair<int, int>> currentPath;
    std::vector<std::pair<int, int>> patrolTargets;
    MapData& mapData = MapData::getInstance();
    NavigationMap* navigationMap = nullptr;
    Transform* mAITankTransform = nullptr;
    RigidBody* mAITankRigidBody = nullptr;
    GameObject* mTargetTank = nullptr;
    Transform* mAIBatteryTransform = nullptr;
    Transform* mAIBatteryPointTransform = nullptr;
    AudioSource* mAudioSource = nullptr;
    AICounter* aiCounter = nullptr;
    Buffable* mBuffable = nullptr;
    TextTGUI* mTextAiNum = nullptr;
    const float rateOfFire= 2.0f;
    
    void updateMovement();
    void tankStateMachine();
    void track();
    void wander();
    void attackTarget();
    void fire();
    void handlePatrol();
    void resetBattery();
};
