#include "TankController.h"

#include "TankPlayer.h"
#include "Engine/Component/RenderComponent/MeshFilter.h"
#include "Engine/Component/RenderComponent/MeshRenderer.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/InputControl/TankinInput.h"
#include "Engine/Utility/GameTime/GameTime.h"
#include "GamePlay/Script/Numeric/Buff/Buffs/ChangeArmorBuff.h"
#include "GamePlay/Script/Numeric/Buff/Buffs/DamageBuff.h"
#include "GamePlay/Script/Numeric/Buff/Buffs/HealBuff.h"
#include "GamePlay/Script/Numeric/Component/BuffAttacher.h"
#include "GamePlay/Script/Shell/ShellController.h"
#include "GamePlay/Script/Shell/ShellFactory.h"
#include "TankCameraController.h"
#include "GamePlay/Script/Enemy/AICounter.h"

REGISTER_COMPONENT(TankController, "TankController")

void TankController::awake()
{
    mTankTransform = mGameObject->getTransform();
    mTankRigidBody = dynamic_cast<RigidBody*>(mGameObject->getComponent("RigidBody"));
    mLifeRect = dynamic_cast<RectTransform*>(Transform::sFindGameObject("PlayerHP"));
    mTextCd = dynamic_cast<TextTGUI*>(Transform::sFindGameObject("TextCd")->getGameObject()->getComponent("TextTGUI"));
    
    mParticleSystem = dynamic_cast<ParticleSystem*>(
        mTankTransform->getChildWithName("TankParticle")->getGameObject()->getComponent("ParticleSystem"));
    mParticleSystem->stopGenerate();

    mBatteryTransform = mTankTransform->getChildWithName("TankBattery");
    mBatteryPointTransform = mTankTransform->getChildWithName("BatteryPoint");
    mGunTransform = mTankTransform->getChildWithName("TankGun");
    
    mAudioSource = dynamic_cast<AudioSource*>(mGameObject->getComponent("AudioSource"));
    mAudioSource->addAuidioClip("airConditioner", FileManager::sGetLoadedBolbFile<AudioClip*>("airConditioner"));
    mAudioSource->addAuidioClip("smallEngineLoop", FileManager::sGetLoadedBolbFile<AudioClip*>("smallEngineLoop"));
    mAudioSource->setVolume("smallEngineLoop", 0.1);
    mAudioSource->addAuidioClip("subsachine_shot", FileManager::sGetLoadedBolbFile<AudioClip*>("subsachine_shot"));
    mAudioSource->addAuidioClip("destroy", FileManager::sGetLoadedBolbFile<AudioClip*>("destroy"));
    mAudioSource->addAuidioClip("BossBGM", FileManager::sGetLoadedBolbFile<AudioClip*>("BossBGM"));
    mAudioSource->addAuidioClip("CommonBGM", FileManager::sGetLoadedBolbFile<AudioClip*>("CommonBGM"));

    TankPlayer::sGetInstanse()->refreshSelf();

    mBuffable = dynamic_cast<Buffable*>(mGameObject->getComponent("Buffable"));
    mBuffable->setBaseNumericalObject(TankPlayer::sGetInstanse()->mNumericalObject);

    mAmingLine = dynamic_cast<AmingLine*>(mGameObject->getComponent("AmingLine"));
    
    setAndChangeTankModel();

    if (AICounter::getInstance()->getLevel() == Level::LEVEL_BOSS)
    {
        mAudioSource->playLoop("BossBGM");
    }
    else
    {
        mAudioSource->playLoop("CommonBGM");
    }
}

void TankController::setAndChangeTankModel()
{
    mBodyMeshFilter = dynamic_cast<MeshFilter*>(mTankTransform->getChildWithName("TankBody")->getGameObject()->getComponent("MeshFilter"));
    mBodyMeshRenderer = dynamic_cast<MeshRenderer*>(mTankTransform->getChildWithName("TankBody")->getGameObject()->getComponent("MeshRenderer"));

    mTankBodyTransform = mBodyMeshFilter->getGameObject()->getTransform();
    
    mBatteryMeshFilter = dynamic_cast<MeshFilter*>(mTankTransform->getChildWithName("TankBattery")->getGameObject()->getComponent("MeshFilter"));
    mBatteryMeshRenderer = dynamic_cast<MeshRenderer*>(mTankTransform->getChildWithName("TankBattery")->getGameObject()->getComponent("MeshRenderer"));

    mTrackMeshFilter = dynamic_cast<MeshFilter*>(mTankTransform->getChildWithName("TankTrack")->getGameObject()->getComponent("MeshFilter"));
    mTrackMeshRenderer = dynamic_cast<MeshRenderer*>(mTankTransform->getChildWithName("TankTrack")->getGameObject()->getComponent("MeshRenderer"));

    mGunMeshFilter = dynamic_cast<MeshFilter*>(mTankTransform->getChildWithName("TankGun")->getGameObject()->getComponent("MeshFilter"));
    mGunMeshRenderer = dynamic_cast<MeshRenderer*>(mTankTransform->getChildWithName("TankGun")->getGameObject()->getComponent("MeshRenderer"));

    switch (TankPlayer::sGetInstanse()->getTankType())
    {
        case TankType::TT_COMMON:
            {
                mBodyMeshFilter->setMesh("CommonBody");
                mBodyMeshRenderer->setTexture("TankTex");
                mBodyMeshRenderer->setBlendFactor(0);

                mBatteryMeshFilter->setMesh("CommonBattery");
                mBatteryMeshRenderer->setTexture("TankTex");
                mBatteryMeshRenderer->setBlendFactor(0);

                mGunMeshFilter->getGameObject()->deactiveGameObject();

                mTrackMeshFilter->setMesh("CommonTrack");
                mTrackMeshRenderer->setTexture("TankTrack");
                mTrackMeshRenderer->setBlendFactor(0);
                break;
            }
        case TankType::TT_HEAVY:
            {
                mBodyMeshFilter->setMesh("AMBT_BODY");
                mBodyMeshRenderer->setTexture("AMBT_BODY");
                mBodyMeshRenderer->setBlendFactor(0);

                mBatteryMeshFilter->setMesh("AMBT_TURRET");
                mBatteryMeshRenderer->setTexture("AMBT_TURRET");
                mBatteryMeshRenderer->setBlendFactor(0);

                mGunMeshFilter->getGameObject()->activeGameObject();
                mGunMeshFilter->setMesh("AMBT_GUN");
                mGunMeshRenderer->setTexture("AMBT_GUN");

                mTrackMeshFilter->setMesh("AMBT_TRACK");
                mTrackMeshRenderer->setTexture("AMBT_TRACK");
                mTrackMeshRenderer->setBlendFactor(0);

                //添加重甲buff
                ChangeArmorBuff* newBuff = new ChangeArmorBuff();
                newBuff->mArmorType = ArmorType::AT_HEAVY;
                mBuffable->attachBuff({newBuff});
                delete newBuff;
                
                break;
            }
        case TankType::TT_LIGHT:
            {
                //更新模型颜色
                mBodyMeshFilter->setMesh("CommonBody");
                mBodyMeshRenderer->setTexture("TankTex");
                mBodyMeshRenderer->setColor({1,0,0});
                mBodyMeshRenderer->setBlendFactor(0.3);

                mBatteryMeshFilter->setMesh("CommonBattery");
                mBatteryMeshRenderer->setTexture("TankTex");
                mBatteryMeshRenderer->setColor({1,0,0});
                mBatteryMeshRenderer->setBlendFactor(0.3);

                mGunMeshFilter->getGameObject()->deactiveGameObject();

                mTrackMeshFilter->setMesh("CommonTrack");
                mTrackMeshRenderer->setTexture("TankTrack");
                mTrackMeshRenderer->setColor({1,0,0});
                mTrackMeshRenderer->setBlendFactor(0.3);

                //添加轻甲buff
                ChangeArmorBuff* newBuff = new ChangeArmorBuff();
                newBuff->mArmorType = ArmorType::AT_LIGHT;
                mBuffable->attachBuff({newBuff});
                delete newBuff;
                
                break;
            }
        default:
            ASSERT(false, TEXT("Unknown TankType!"))
    }
}

 
void TankController::start()
{
    
}

void TankController::update()
{
    TankPlayer::sGetInstanse()->mTime += GameTime::sGetDeltaTime();
    NumericalObject* numerical = mBuffable->getResultNumerical();
    float moveSpeelCorrect = numerical->getAttribute(AttributeType::AT_MOVE_FACTOR);
    float moveForce = 8.0f;
    float moveSpeed = 4.0f * moveSpeelCorrect;
    float sprintSpeed = 8.0f * moveSpeelCorrect;
    float rotationSpeed = 40.0f;
    float maxPitchAngleDeg = 10.0f;
    Vector3 forward = mTankBodyTransform->getForward();
    Vector2 leftStickInput = TankinInput::sGetInstance()->getInputAxis(AxisType::LEFT_STICK);
    Vector2 rightStickInput = TankinInput::sGetInstance()->getInputAxis(AxisType::RIGHT_STICK);

    if (TankinInput::sGetInstance()->isKeyPressed(Action::MOVE_FORWARD) && !GameTime::sGetIsPaused())
    {
        mAudioSource->playLoop("smallEngineLoop");
        mParticleSystem->startGenerate();
        mParticleSystem->mAcceleration = {0, 5, -10};
        Vector3 currentVelocity = mTankRigidBody->getVelocity();
        float currentSpeed = currentVelocity.Length();
        Vector3 batteryForward = mBatteryTransform->getForward();
        // move forward
        if (currentSpeed < moveSpeed)
        {
            mTankRigidBody->applyForce(forward * moveForce * mTankRigidBody->getMass());
        }
        //检测是否需要缓慢旋转
        /*if (Vector3::Angle(batteryForward, forward) > 0.01f)
        {
            Vector3 cross = batteryForward.CrossProduct(forward);
            mTankTransform->rotateAroundLocalAxis(cross * -1, rotationSpeed * 0.8 * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD);
            mBatteryTransform->rotateAroundLocalAxis(cross, rotationSpeed * 0.8 * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD);
            Vector3 forward = mTankTransform->getForward();
            // 更新刚体的速度,保持转向时速度向车头。
            float currentSpeed = mTankRigidBody->getVelocity().Length();
            Vector3 newVelocity = forward * currentSpeed;
            mTankRigidBody->setVelocity(newVelocity);
        }*/
    }
    
    if (TankinInput::sGetInstance()->isKeyPressed(Action::MOVE_BACK) && !GameTime::sGetIsPaused())
    {
        mAudioSource->playLoop("smallEngineLoop");
        mParticleSystem->startGenerate();
        Vector3 currentVelocity = mTankRigidBody->getVelocity();
        float currentSpeed = currentVelocity.Length();
        // move forward
    	if (currentSpeed < moveSpeed)
    	{
    		mTankRigidBody->applyForce(forward * moveForce * mTankRigidBody->getMass() * -1);
    	}

        Vector3 batteryForward = mBatteryTransform->getForward();

        //检测是否需要缓慢旋转
        /*if (Vector3::Angle(batteryForward, forward) > 0.01f)
        {
            Vector3 cross = batteryForward.CrossProduct(forward);
            mTankTransform->rotateAroundLocalAxis(cross * -1, rotationSpeed * 0.8 * GameTime::sGetFixedDeltaTime() * MathUtils::DEG_TO_RAD);
            mBatteryTransform->rotateAroundLocalAxis(cross, rotationSpeed * 0.8 * GameTime::sGetFixedDeltaTime() * MathUtils::DEG_TO_RAD);
            Vector3 forward = mTankTransform->getForward();
            // 更新刚体的速度,保持转向时速度向车头。
            float currentSpeed = mTankRigidBody->getVelocity().Length();
            Vector3 newVelocity = forward * currentSpeed;
            mTankRigidBody->setVelocity(newVelocity*-1);
        }*/
    }
    
    if (TankinInput::sGetInstance()->isKeyUp(Action::MOVE_FORWARD) || TankinInput::sGetInstance()->isKeyUp(Action::MOVE_BACK))
    {
        // move forward
        mAudioSource->stop("smallEngineLoop");
        mParticleSystem->stopGenerate();
        mTankRigidBody->setVelocity({0,0,0});
    }

    if (mAttackCd <= 0 && TankinInput::sGetInstance()->isKeyDown(Action::FIRE) && !GameTime::sGetIsPaused())
    {
        GameObject* shell = nullptr;
        if (TankPlayer::sGetInstanse()->getBulletType() == BulletType::Burn)
        {
            shell = ShellFactory::sCreateBurnBullet(mBatteryPointTransform, mBuffable, "Enemy");
        }
        else
        {
            shell = ShellFactory::sCreateCommonBullet(mBatteryPointTransform, mBuffable, "Enemy");
        }
        mAttackCd = TankPlayer::sGetInstanse()->getAttackCd();
        TankPlayer::sGetInstanse()->changeTankPlayerState(TankPlayerState::TP_ATTACKING);
        mAudioSource->play("subsachine_shot");

        //判断是否需要cutscene
        GameObject* amingTarget = mAmingLine->getAmingTarget();
        BuffAttacher* buffAttacher = dynamic_cast<BuffAttacher*>(shell->getComponent("BuffAttacher"));
        if (amingTarget != nullptr && amingTarget->getTag()=="Enemy")
        {
            Buffable* buffable = dynamic_cast<Buffable*>(amingTarget->getComponent("Buffable"));
            if (buffAttacher->damage >= buffable->getCurrentLife() &&
                buffable->isDie() == false)
            {
                TankPlayer::sGetInstanse()->tankCameraController->followBullet(shell->getTransform());
                GameTime::sSetTimeScale(0.1);
            }
        }
    }
    else if (mAttackCd >= 0)
    {
        mAttackCd -= GameTime::sGetDeltaTime();
    }
    if (mAttackCd <= 0)
    {
        if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
        {
            mTextCd->setText("开火剩余冷却时间: 就绪");
        }
        else
        {
            mTextCd->setText("Remaining cooldown time for firing: Ready");
        }
    }
    else
    {
        if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
        {
            mTextCd->setText(TpString("开火剩余冷却时间: ") + std::to_string(mAttackCd) + "s");
        }
        else
        {
            mTextCd->setText(TpString("Remaining cooldown time for firing: ") + std::to_string(mAttackCd) + "s");
        }
    }

    //绘制瞄准线
    mAmingLine->drawLine(mBatteryPointTransform->getWorldPosition(),
            mBatteryPointTransform->getForward(), 50, 0.02);
    
    if (leftStickInput.v.x < 0 && !GameTime::sGetIsPaused())
    {
        // rotate left
        mTankBodyTransform->rotateAroundLocalAxis({0,1,0}, rotationSpeed * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD * leftStickInput.v.x);
        Vector3 forward = mTankBodyTransform->getForward();
        Vector3 currentVelocity = mTankRigidBody->getVelocity();
        // 更新刚体的速度,保持转向时速度向车头。
        float currentSpeed = mTankRigidBody->getVelocity().Length();
        float sign = (currentVelocity.Dot(forward) >= 0) ? 1.0f : -1.0f;
        Vector3 newVelocity = forward * currentSpeed * sign;
        mTankRigidBody->setVelocity(newVelocity);
    }
    if (leftStickInput.v.x > 0 && !GameTime::sGetIsPaused())
    {
        
        mTankBodyTransform->rotateAroundLocalAxis({0,1,0}, rotationSpeed * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD * leftStickInput.v.x);
        Vector3 forward = mTankBodyTransform->getForward();
        Vector3 currentVelocity = mTankRigidBody->getVelocity();
        // 更新刚体的速度,保持转向时速度向车头。
        float currentSpeed = mTankRigidBody->getVelocity().Length();
        float sign = (currentVelocity.Dot(forward) >= 0) ? 1.0f : -1.0f;
        Vector3 newVelocity = forward * currentSpeed * sign;
        mTankRigidBody->setVelocity(newVelocity);
    }

    if (rightStickInput.v.x < 0)
    {
        mBatteryTransform->rotateAroundLocalAxis({0,1,0}, rightStickInput.v.x * rotationSpeed * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD);
    }
    if (rightStickInput.v.x > 0)
    {
        mBatteryTransform->rotateAroundLocalAxis({0,1,0}, rightStickInput.v.x * rotationSpeed * GameTime::sGetFixedDeltaTime()* MathUtils::DEG_TO_RAD);
    }
    if (TankPlayer::sGetInstanse()->getTankType() == TankType::TT_HEAVY && rightStickInput.v.y > 0)
    {
        Vector3 localRotation = mGunTransform->getLocalRotation().getEulerAnglesDegree();
        if (localRotation.v.x < maxPitchAngleDeg)
        {
            mGunTransform->rotateAroundLocalAxis({1,0,0}, -rightStickInput.v.y  * rotationSpeed * GameTime::sGetFixedDeltaTime()*MathUtils::DEG_TO_RAD);
        }
        
    }
    if (TankPlayer::sGetInstanse()->getTankType() == TankType::TT_HEAVY && rightStickInput.v.y< 0)
    {
        Vector3 localRotation = mGunTransform->getLocalRotation().getEulerAnglesDegree();
        if (localRotation.v.x > 0)
        {
            mGunTransform->rotateAroundLocalAxis({1,0,0}, -rightStickInput.v.y * rotationSpeed * GameTime::sGetFixedDeltaTime()*MathUtils::DEG_TO_RAD);
        }
    }

    //更新血条显示
    mLifeRect->setWidth(mLifeWidth*mBuffable->getLifePecentage());
    
    //死亡
    if (mBuffable->getLifePecentage() <= 0)
    {
        Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/EndScene.xml");
        mAudioSource->play("destroy");
    }

    //呼吸回血
    breatheHeal();
}

void TankController::fixedUpdate()
{
   
}

void TankController::breatheHeal()
{
    //呼吸回血cd
    static const float breathCd = 5;
    static float currentCd = breathCd;

    if (TankPlayer::sGetInstanse()->mIsAttacked == TankAttackedState::TS_ATTACKING)
    { //本帧受击，重置呼吸回血cd,将状态改为之前受击状态
        currentCd = breathCd;
        TankPlayer::sGetInstanse()->mIsAttacked = TankAttackedState::TS_ATTACKED;
    }
    
    if (TankPlayer::sGetInstanse()->mIsAttacked == TankAttackedState::TS_ATTACKED)
    {
        currentCd -= GameTime::sGetDeltaTime();
        if (currentCd <0)
        {
            //冷却结束，将状态改为none
            TankPlayer::sGetInstanse()->mIsAttacked = TankAttackedState::TS_NONE;
        }
    }
    
    if (mBuffable->getLifePecentage() < 1 && TankPlayer::sGetInstanse()->mIsAttacked == TankAttackedState::TS_NONE)
    {
        HealBuff* newBuff = new HealBuff();
        newBuff->mHealValue = 0.05;
        mBuffable->attachBuff({newBuff});
    }
}
