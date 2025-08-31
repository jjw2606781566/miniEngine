#include "AIController.h"


#include "AICounter.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Component/RenderComponent/MeshFilter.h"
#include "Engine/Component/RenderComponent/MeshRenderer.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/math/math.h"
#include "Engine/Physical/PhysicSystem.h"
#include "Engine/Utility/GameTime/GameTime.h"
#include "GamePlay/Script/Player/TankPlayer.h"
#include "GamePlay/Script/Shell/ShellFactory.h"

REGISTER_COMPONENT(AIController, "AIController")

void AIController::awake()
{
    mEnemyState = EnemyState::ES_PATROL;
    active = true;
    isMoving = false;
    canHitTarget = false;
    attackTimer = 0.0f;
    pathFindingTimer = 0.0f;
    stopRange = 2.0f;
    patrolPointIndex = 0;
    guardRange = 35; 
    fireRange = 25;
    destination = std::make_pair(-1, -1);
    //--------------------------------------------- 指针初始化 ----------------------------------------------------
    navigationMap = NavigationMap::getInstance();
    mAITankTransform = getGameObject()->getTransform();
    mAITankRigidBody = dynamic_cast<RigidBody*>(getGameObject()->getComponent("RigidBody"));
    mTargetTank = Transform::sFindGameObject("TankRoot")->getGameObject();
    aiCounter = AICounter::getInstance();
    mTextAiNum = dynamic_cast<TextTGUI*>(Transform::sFindGameObject("TextAiNumber")->getGameObject()->getComponent("TextTGUI"));
    //----------------------------------------------- 声音设置 ---------------------------------------------------------
    mAudioSource = dynamic_cast<AudioSource*>(mGameObject->getComponent("AudioSource"));
    // auto clip1 = FileManager::sGetLoadedBolbFile<AudioClip*>("airConditioner");
    auto clip2 = FileManager::sGetLoadedBolbFile<AudioClip*>("subsachine_shot");
    auto clip3 = FileManager::sGetLoadedBolbFile<AudioClip*>("destroy");
    auto clip4 = FileManager::sGetLoadedBolbFile<AudioClip*>("smallEngineLoop");
    auto clip5 = FileManager::sGetLoadedBolbFile<AudioClip*>("carDrivingLoop");
    // mAudioSource->addAuidioClip("airConditioner", clip1);
    mAudioSource->addAuidioClip("subsachine_shot", clip2);
    mAudioSource->addAuidioClip("destroy", clip3);
    mAudioSource->addAuidioClip("smallEngineLoop", clip4);
    mAudioSource->addAuidioClip("carDrivingLoop", clip5);
    mAudioSource->addAuidioClip("fire", FileManager::sGetLoadedBolbFile<AudioClip*>("fire"));
    // mAudioSource->set3DMode("airConditioner",true);
    mAudioSource->set3DMode("subsachine_shot", true);
    mAudioSource->set3DMode("destroy", true);
    mAudioSource->set3DMode("smallEngineLoop", true);
    mAudioSource->set3DMode("carDrivingLoop", true);
    mAudioSource->set3DMode("fire",true);
    // mAudioSource->setVolume("airConditioner", 0.25);
    mAudioSource->setVolume("destroy", 0.5);
    mAudioSource->setVolume("smallEngineLoop", 0.3);
    // mAudioSource->playLoop("airConditioner");
    mAudioSource->playLoop("smallEngineLoop");
    mAudioSource->playLoop("carDrivingLoop");
    srand(static_cast<unsigned>(time(nullptr)));
    //-------------------------------------------- 子弹相关属性 -----------------------------------------------------------
    mBuffable = dynamic_cast<Buffable*>(mGameObject->getComponent("Buffable"));
    maxHealth = mBuffable->getLifePecentage();
}

void AIController::start()
{
    navigationMap->setAiMap(&mapData);
    mAIBatteryTransform = mAITankTransform->getChildWithName("TankBattery");
    mAIBatteryPointTransform = mAITankTransform->getChildWithName("BatteryPoint");
    // 全局计数
    aiCounter->increaceValue();
}

void AIController::update()
{
    if (!active) return;

    navigationMap->setAiMap(&mapData);

    // 攻击冷却
    if (attackTimer > 0.0f)
        attackTimer -= GameTime::sGetDeltaTime();
    if (attackTimer < 0.0f)
        attackTimer = 0.0f;

    if (pathFindingTimer > 0.0f)
        pathFindingTimer -= GameTime::sGetDeltaTime();
    if (pathFindingTimer < 0.0f)
        pathFindingTimer = 0.0f;

    updateMovement();
    
    tankStateMachine();

    if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
    {
        mTextAiNum->setText(TpString("剩余AI数量: ") + std::to_string(aiCounter->getValue()));
    }
    else
    {
        mTextAiNum->setText(TpString("Remaining number of AI: ") + std::to_string(aiCounter->getValue()));
        
    }
}

void AIController::onDestory()
{
    //aiCounter->decreaseValue();
    // 如果AI全部死亡，游戏结束
    if (aiCounter->getValue() <= 14)
    {
        aiCounter->nextLevel();
    }
}

void AIController::tankMoveTo(int targetX, int targetZ)
{
    // 控制pathFinding的频率
    if (pathFindingTimer > 0.0f)
    {
        return;
    }
    
    destination = std::make_pair(targetX, targetZ);
    Vector3 currentPos = mAITankTransform->getWorldPosition();
    
    // 将世界坐标转换为网格坐标
    int currentGridX = static_cast<int>((currentPos.v.x - 1) / 2);
    int currentGridZ = static_cast<int>((currentPos.v.z - 1) / 2);
    if (currentGridX < 0) currentGridX = 0;
    if (currentGridZ < 0) currentGridZ = 0;
    
    currentPath = navigationMap->pathFinding(currentGridX, currentGridZ, targetX, targetZ);
    isMoving = !currentPath.empty();
    pathFindingTimer = pathFindingInterval;
}

void AIController::setStopRanger(float range)
{
    stopRange = range;
}

void AIController::setActive(bool a)
{
    active = a;
}

void AIController::setPatrol(bool p)
{
    isPatrolling = p;
}

void AIController::addPatrolTarget(int targetX, int targetZ)
{
    patrolTargets.push_back(std::pair<int, int>(targetX, targetZ));
    DEBUG_PRINT("AIController: Added patrol target (%d, %d)", targetX, targetZ);
}

void AIController::removePatrolTarget(int targetX, int targetZ)
{
    auto it = std::find(patrolTargets.begin(), patrolTargets.end(), std::pair<int, int>(targetX, targetZ));
    if (it != patrolTargets.end())
    {
        patrolTargets.erase(it);
        DEBUG_PRINT("AIController: Removed patrol target (%d, %d)", targetX, targetZ);
    }
    else
    {
        DEBUG_PRINT("AIController: Failed to remove patrol target (%d, %d) - target not found", targetX, targetZ);
    }
}

void AIController::clearPatrolTargets()
{
    patrolTargets.clear();
    DEBUG_PRINT("AIController: Cleared all patrol targets");
}

void AIController::attackTarget()
{
    Vector3 currentPosition = mAITankTransform->getWorldPosition();
    Vector3 toTarget = targetPos - currentPosition;
    toTarget.v.y = 0;
    toTarget = toTarget.Normalize();

    Vector3 forwardDirection = mAITankTransform->getForward();

    float dot = forwardDirection.Dot(toTarget);
    if (dot > 1.0f) dot = 1.0f;
    else if (dot < -1.0f) dot = -1.0f;
    float angle = acosf(dot);

    // 坦克本体旋转向目标
    if (angle > 1.0f) {
        Vector3 cross = forwardDirection.CrossProduct(toTarget);
        Vector3 up = mAITankTransform->getUp();
        float rotateDir = (cross.Dot(up) > 0.f) ? 1.f : -1.f;

        float deltaTime = GameTime::sGetFixedDeltaTime();
        float rotateAmount = rotationSpeed * deltaTime;
        float actualRotate = (angle < rotateAmount) ? angle : rotateAmount;

        mAITankTransform->rotateAroundWorldAxis(up, actualRotate * rotateDir);
    }

    bool batteryAim = false;

    if (mAIBatteryTransform)
    {
        // 计算炮台
        Vector3 batteryForward = mAIBatteryTransform->getForward();
        // 目标相对炮台的方向
        Vector3 batteryPos = mAIBatteryTransform->getWorldPosition();
        Vector3 toTargetFromBattery = targetPos - batteryPos;
        toTargetFromBattery.v.y = 0;
        toTargetFromBattery = toTargetFromBattery.Normalize();

        float batteryDot = batteryForward.Dot(toTargetFromBattery);
        if (batteryDot > 1.0f) batteryDot = 1.0f;
        else if (batteryDot < -1.0f) batteryDot = -1.0f;
        float batteryAngle = acosf(batteryDot);

        if (batteryAngle > 0.01f)
        {
            Vector3 batteryCross = batteryForward.CrossProduct(toTargetFromBattery);
            float batteryRotateDir = (batteryCross.v.y > 0) ? 1.f : -1.f;

            float batteryRotateAmount = rotationSpeed * GameTime::sGetFixedDeltaTime();
            float batteryActualRotate = (batteryAngle < batteryRotateAmount) ? batteryAngle : batteryRotateAmount;

            mAIBatteryTransform->rotateAroundLocalAxis({0, 1, 0}, batteryActualRotate * batteryRotateDir);
        }
        constexpr float fireAngle = 0.05f;
        if (batteryAngle < fireAngle)
        {
            batteryAim = true;
        }
    }
    

    if (attackTimer > 0.0f)
    {
        return;
    }

    // 面对目标时开火
    if (batteryAim && canHitTarget) {
        fire();
        attackTimer = rateOfFire;
    }
}

void AIController::fire()
{
    if(mBuffable)
    {
        mAudioSource->play("subsachine_shot");
        ShellFactory::sCreateCommonBullet(mAIBatteryPointTransform, mBuffable, "Player");
    }
}

bool AIController::isActive() const
{
    return active;
}

void AIController::stop()
{
    isMoving = false;
    currentPath.clear();
    DEBUG_PRINT("AIController: Stopped moving");
}

rapidxml::xml_node<>* AIController::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("AIController")));
    mXmlNode->append_attribute(doc->allocate_attribute("active", doc->allocate_string(active ? "1" : "0")));
    mXmlNode->append_attribute(doc->allocate_attribute("isPatrolling", doc->allocate_string(isPatrolling ? "1" : "0")));
    mXmlNode->append_attribute(doc->allocate_attribute("patrolPointIndex", doc->allocate_string(std::to_string(patrolPointIndex).c_str())));
    
    std::string patrolTargetsStr;
    for (const auto& target : patrolTargets) {
        if (!patrolTargetsStr.empty()) patrolTargetsStr += "|"; // 用 | 分隔不同目标
        patrolTargetsStr += std::to_string(target.first) + "," + std::to_string(target.second);
    }
    mXmlNode->append_attribute(doc->allocate_attribute("patrolTargets", doc->allocate_string(patrolTargetsStr.c_str())));
    return mXmlNode;
}

void AIController::deSerialize(const rapidxml::xml_node<>* node)
{
    active = std::string(node->first_attribute("active")->value()) == "1";
    bool patrolling = std::string(node->first_attribute("isPatrolling")->value()) == "1";
    setPatrol(patrolling);
    patrolPointIndex = std::stof(node->first_attribute("patrolPointIndex")->value());

    // 解析patrolTargets
    patrolTargets.clear();
    auto patrolTargetsAttr = node->first_attribute("patrolTargets");
    if (patrolTargetsAttr) {
        std::string patrolTargetsStr = patrolTargetsAttr->value();
        std::istringstream iss(patrolTargetsStr);
        std::string targetStr;
        
        // 按 | 分割字符串
        while (std::getline(iss, targetStr, '|')) {
            // 按 , 分割坐标
            size_t commaPos = targetStr.find(',');
            if (commaPos != std::string::npos) {
                int x = std::stoi(targetStr.substr(0, commaPos));
                int z = std::stoi(targetStr.substr(commaPos + 1));
                addPatrolTarget(x, z);
            }
        }
    }
}

void AIController::setMoveSpeed(float speed)
{
    maxSpeed = speed;
}

void AIController::setMoveForce(float force)
{
    moveForce = force;
}

void AIController::setRotationSpeed(float speed)
{
    rotationSpeed = speed;
}

void AIController::setPathFindingInterval(float interval)
{
    pathFindingInterval = interval;
}


void AIController::updateMovement()
{
    if (Application::sGetRunningType() == EngineRunningType::Editor) return;
    if (!isMoving) return;

    if (currentPath.empty()) return;
    // ---------------------------------------- 移动位置 -----------------------------------------------
    // 获取当前位置和下个路径点
    Vector3 currentPos = mAITankTransform->getWorldPosition().v;
    auto frontPair = currentPath.front();
            
    // 网格坐标转换为世界坐标
    float xWorld = frontPair.first * 2 + 1;
    float zWorld = frontPair.second * 2 + 1;
    Vector3 targetPos(xWorld, currentPos.v.y, zWorld);

    Vector3 direction = targetPos - currentPos;
    float distance = direction.Length();

    // 到达路径点后移除它
    if (distance < stopRange)
    {
        currentPath.erase(currentPath.begin());
        if (currentPath.empty())
        {
            stop();
            return;
        }
        frontPair = currentPath.front();
        float xWorld = frontPair.first * 2 + 1;
        float zWorld = frontPair.second * 2 + 1;
        targetPos = Vector3(xWorld, currentPos.v.y, zWorld);
        direction = targetPos - currentPos;
    }
    //------------------------------------------ 旋转 ------------------------------------------------
    Vector3 targetDirection = direction.Normalize();
    // 旋转修正
    Vector3 curForward = mAITankTransform->getForward();
    float dot = curForward.Dot(targetDirection);
    dot = (dot > 1.0f) ? 1.0f : (dot < -1.0f) ? -1.0f : dot;
    float angle = acosf(dot);

    if (angle > 0.1f) { 
        Vector3 cross = curForward.CrossProduct(targetDirection);
        Vector3 up = mAITankTransform->getUp();
        float rotateDirection = (cross.Dot(up) > 0) ? 1.0f : -1.0f;
        
        float deltaTime = GameTime::sGetFixedDeltaTime();
        float rotateAmount = rotationSpeed * deltaTime;
        float actualRotate = (angle < rotateAmount) ? angle : rotateAmount;
        
        mAITankTransform->rotateAroundWorldAxis(
            up, 
            actualRotate * rotateDirection
        );
        
        DEBUG_PRINT("AIController::updateMovement() - Rotating by %.4f radians (%.2f degrees)", 
           actualRotate * rotateDirection, 
           actualRotate * rotateDirection * 57.2958f);
        // 180/π 57.2958f
    }
    
    // ---------------------------------------- 移动控制 --------------------------------------------------
    if (mAITankRigidBody) {
        Vector3 curVelocity = mAITankRigidBody->getVelocity();
        float curSpeed = curVelocity.Length();
        
        // 计算方向对齐度
        float alignFactor = curForward.Dot(targetDirection);
        
        // 应用主推进力
        float thrust = 0.2f + 0.8f * (alignFactor > 0 ? alignFactor : 0);
        if (curSpeed < maxSpeed * thrust) {
            Vector3 thrustForce = curForward * moveForce * thrust * mAITankRigidBody->getMass();
            mAITankRigidBody->applyForce(thrustForce);
        }
        
        // 计算并应用侧向摩擦力以减少漂移
        Vector3 forwardVelocity = curForward * curVelocity.Dot(curForward);
        Vector3 lateralVelocity = curVelocity - forwardVelocity;
        
        if (lateralVelocity.Length() > 0.1f) {
            // 侧向摩擦系数
            float lateralFrict = 2.5f;
           
            Vector3 lateralFrictDirect = lateralVelocity.Normalize();
            lateralFrictDirect = lateralFrictDirect * (-1.0f); // 反方向
            float lateralFrictM = lateralVelocity.Length() * lateralFrict * mAITankRigidBody->getMass();
            Vector3 lateralFrictionForce = lateralFrictDirect * lateralFrictM;
            mAITankRigidBody->applyForce(lateralFrictionForce);
        }
    }
}

void AIController::tankStateMachine()
{
    Vector3 currentPosition = mAITankTransform->getWorldPosition();

    // 死亡
    if (mEnemyState != EnemyState::ES_DEAD && mBuffable->isDie())
    {
        mEnemyState = EnemyState::ES_DEAD;
        mAudioSource->play("destroy");
        getGameObject()->addComponent("Obstacle");
        aiCounter->decreaseValue();
        if (aiCounter->getLevel() == Level::LEVEL_COMMON)
        {
            TankPlayer::sGetInstanse()->mScore += 100;
        }
        else
        {
            TankPlayer::sGetInstanse()->mScore += 5000;
        }
        
    }

    // 存活
    if (mEnemyState != EnemyState::ES_DEAD)
    {
        if (mTargetTank)
        {
            targetPos = mTargetTank->getTransform()->getWorldPosition();

            Vector3 rayOrigin = mAIBatteryPointTransform->getWorldPosition();
            Vector3 direction = targetPos - rayOrigin;
            direction = direction.Normalize();

            // 射线检测
            RaycastHit hit;
            bool hitDetected = PhysicSystem::getInstance().raycast(rayOrigin, direction, guardRange, hit);
            canHitTarget = hitDetected && hit.body == dynamic_cast<RigidBody*>(mTargetTank->getComponent("RigidBody"));
            squaredDisTarget = (targetPos - currentPosition).LengthSquared();

            // 如果生命受损(被攻击),
            auto n = mBuffable->getLifePecentage();
            
            // 更新状态机
            if ((squaredDisTarget > guardRange * guardRange || !canHitTarget) && maxHealth == n) {
                if (isPatrolling)
                {
                    mEnemyState = EnemyState::ES_PATROL;
                }
                else
                {
                    mEnemyState = EnemyState::ES_WANDER;
                }
            }
            else if (squaredDisTarget > fireRange * fireRange) {
                mEnemyState = EnemyState::ES_TRACK;
            } else {
                mEnemyState = EnemyState::ES_ATTACK;
            }
        }
        else
        {
            mEnemyState = EnemyState::ES_PATROL;
        }
    }

    // 执行不同状态行为逻辑
    switch (mEnemyState)
    {
    case EnemyState::ES_PATROL:
        resetBattery();
        handlePatrol();
        break;

    case EnemyState::ES_TRACK:
        resetBattery();
        track();    
        break;
    
    case EnemyState::ES_ATTACK:
        attackTarget();
        break;

    case EnemyState::ES_WANDER:
        resetBattery();
        wander();
        break;

    case EnemyState::ES_DEAD:
        stop();
        mAudioSource->stop("smallEngineLoop");
        mAudioSource->stop("carDrivingLoop");
        break;
    }
}

void AIController::track()
{
    int destX = static_cast<int>((targetPos.v.x - 1) / 2);
    int destZ = static_cast<int>((targetPos.v.z - 1) / 2);
    tankMoveTo(destX, destZ);
}

void AIController::wander()
{
    Vector3 currentPosition = mAITankTransform->getWorldPosition();
    
    // 如果没有目标位置，选择一个随机目标
    if (destination.first == -1 && destination.second == -1)
    {
        std::vector<std::pair<int, int>> availableDestinations = navigationMap->getNavigationMap();
        float wonderRange = 10.0f;  
        
        // 随机目标
        for (int attempts = 0; attempts < 20; attempts++)  
        {
            int i = rand() % availableDestinations.size();
            float destX = availableDestinations[i].first * 2 + 1;
            float destZ = availableDestinations[i].second * 2 + 1;
            
            float distanceSquared = 
                (destX - currentPosition.v.x) * (destX - currentPosition.v.x) + 
                (destZ - currentPosition.v.z) * (destZ - currentPosition.v.z);
            
            if (distanceSquared >= wonderRange * wonderRange)
            {
                destination = availableDestinations[i];
                break;
            }
        }
    }
    
    // 计算到目标的距离
    float destX = destination.first * 2 + 1;
    float destZ = destination.second * 2 + 1;
    float distanceToDestinationSquared = 
        (currentPosition.v.x - destX) * (currentPosition.v.x - destX) + 
        (currentPosition.v.z - destZ) * (currentPosition.v.z - destZ);
    
    // 如果尚未到达目标
    if (distanceToDestinationSquared >= stopRange * stopRange)
    {
        tankMoveTo(destination.first, destination.second);
    }
    else
    {
        destination = std::make_pair(-1, -1);  // 重置目标
        DEBUG_PRINT("AIController: Wait complete, selecting new target");
    }
}


void AIController::handlePatrol()
{
    
    if (patrolTargets.empty()) {
        DEBUG_PRINT("AIController::handlePatrol() - No patrol targets");
        return;
    }
    
    Vector3 currentPosition = mAITankTransform->getWorldPosition();
    std::pair<int, int> patrolTarget = patrolTargets[patrolPointIndex];
    Vector3 targetPos(
        patrolTarget.first * 2 + 1,
        currentPosition.v.y,
        patrolTarget.second * 2 + 1);

    float dist = (currentPosition - targetPos).Length();
    if (dist < stopRange) {
        patrolPointIndex = (patrolPointIndex + 1) % patrolTargets.size();
        isMoving = false;
        patrolTarget = patrolTargets[patrolPointIndex];
    }

    if (!isMoving) {
        tankMoveTo(patrolTarget.first, patrolTarget.second);
        isMoving = true;
    }
}

void AIController::resetBattery()
{
    if (mAIBatteryTransform)
    {
        Vector3 batteryForward = mAIBatteryTransform->getForward();
        Vector3 currentForward = mAITankTransform->getForward();
        float batteryDot = batteryForward.Dot(currentForward);
        if (batteryDot > 1.0f) batteryDot = 1.0f;
        else if (batteryDot < -1.0f) batteryDot = -1.0f;
        float batteryAngle = acosf(batteryDot);

        if (batteryAngle > 0.01f)
        {
            Vector3 batteryCross = batteryForward.CrossProduct(currentForward);
            float batteryRotateDir = (batteryCross.v.y > 0) ? 1.f : -1.f;

            float batteryRotateAmount = rotationSpeed * GameTime::sGetFixedDeltaTime();
            float batteryActualRotate = (batteryAngle < batteryRotateAmount) ? batteryAngle : batteryRotateAmount;

            mAIBatteryTransform->rotateAroundLocalAxis({0, 1, 0}, batteryActualRotate * batteryRotateDir);
        }
    }
}