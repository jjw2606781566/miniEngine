#include "TankCameraController.h"

#include "TankPlayer.h"
#include "Engine/InputControl/TankinInput.h"
#include "Engine/Utility/Random.h"
#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(TankCameraController, "TankCameraController")

void TankCameraController::awake()
{
    mRelativePos = {0,2,-8};
    mCamera = dynamic_cast<Camera*>(mGameObject->getComponent("Camera"));
    mTransform = mGameObject->getTransform();
    mTransform->setWorldPosition(mRelativePos);

    mTankTransform = Transform::sFindGameObject("TankRoot");
    mBatteryPointTransform = mTankTransform->getChildWithName("BatteryPoint");
    mBatteryTransform = mTankTransform->getChildWithName("TankBattery");

    TankPlayer::sGetInstanse()->tankCameraController = this;
}

void TankCameraController::start()
{
    
}

void TankCameraController::update()
{
    if (mIsFollowingBullet)
    {
        //开始跟随子弹
        const Vector3 mRelativePos = {0,2,-6};
        Vector3 finalPos = mBatteryTransform->getForward() * mRelativePos.v.z;
        finalPos += mBatteryTransform->getUp() * mRelativePos.v.y;
        mTransform->setWorldPosition(finalPos + follingBullet->getWorldPosition());
        //mTransform->setWorldRotation(mBatteryTransform->getWorldRotation());
        return;
    }
    Vector3 up = mBatteryPointTransform->getUp();
    Vector3 forward = mBatteryPointTransform->getForward();
    Vector3 tankPos = mBatteryPointTransform->getWorldPosition();

    //计算forward对水平面投影，这样就能在抬起炮管时水平上升相机
    static Vector3 WorldUp = {0,1,0};
    Vector3 normProj = WorldUp * forward.Dot(WorldUp);
    Vector3 forwardProj = forward - normProj;
    forwardProj = forwardProj.Normalize();

    const float maxDistance = -8;
    const float minDistance = -3;
    const float speed = 4;
    //按键更改相机距离
    if (mRelativePos.v.z > maxDistance && TankinInput::sGetInstance()->isKeyPressed(Action::CAMERA_FAR))
    {
        mRelativePos.v.z -= speed*GameTime::sGetDeltaTime();
    }
    else if (mRelativePos.v.z < minDistance && TankinInput::sGetInstance()->isKeyPressed(Action::CAMERA_NEAR))
    {
        mRelativePos.v.z += speed*GameTime::sGetDeltaTime();
    }

    //根据向上看的角度，调整z值，向上看时视野距离应该拉大，也就是将相机拉远
    Vector3 finalRelativePos = mRelativePos;
    float pitchDeg = mBatteryPointTransform->getWorldRotation().getEulerAnglesDegree().v.x;
    finalRelativePos.v.z -= pitchDeg*0.5;
    finalRelativePos.v.y += pitchDeg*0.3;
    //DEBUG_FOCUS_PRINT("pitchDeg: %f\n", pitchDeg)

    //计算新的目标position
    tankPos += (forwardProj * finalRelativePos.v.z);
    tankPos += (up * finalRelativePos.v.y);

    //插值让相机平滑过度
    float lerpFactor = 0.05f;
    Vector3 targetPos = Vector3::Lerp(mTransform->getWorldPosition(), tankPos, lerpFactor);

    //更新相机的位置
    mTransform->setWorldPosition(targetPos);
    mTransform->setLocalRotation(mBatteryPointTransform->getWorldRotation());

    //开始在新位置的基础上进行相机抖动

    if (TankPlayer::sGetInstanse()->getTankPlayerState() == TankPlayerState::TP_ATTACKING)
    {
        // 防止相机过高时震动，高频率渲染
        if (mTransform->getWorldPosition().v.y < shakeHeightLimit)
        {
            startShake(0.2, 0.1);
        }
        TankPlayer::sGetInstanse()->changeTankPlayerState(TankPlayerState::TP_IDLE);
    }
        
    if (mIsShaking)
    {
        mElapsedTime += GameTime::sGetFixedDeltaTime();
        if (mElapsedTime > mShakingTime) {
            mIsShaking = false;
            return;
        }

        // 生成随机偏移量，范围在 [-amplitude, amplitude]
        Vector3 shakeOffset; 
        shakeOffset.v.x = (Random::Float()*2 - 1) * mShakeAmplitude;
        shakeOffset.v.y = (Random::Float()*2 - 1) * mShakeAmplitude;
        shakeOffset.v.z = (Random::Float()*2 - 1) * mShakeAmplitude;

        targetPos += shakeOffset;
        mTransform->setWorldPosition(targetPos);
    }

    //修正相机的rotation，防止它向上看
    mTransform->rotateAroundLocalAxis({1,0,0}, pitchDeg * MathUtils::DEG_TO_RAD);
}


