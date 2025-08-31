#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/ComponentHeader/TankinRenderComponent.h"
#include "Engine/Component/TGUI/RectTransform.h"

class TankCameraController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void followBullet(Transform* bullet)
    {
        mIsFollowingBullet = true;
        follingBullet = bullet;
    }
    void unFollowBullet()
    {
        mIsFollowingBullet = false;
        follingBullet = nullptr;
    }
    Transform* getBulletTransform() const {return follingBullet;}
private:
    Camera* mCamera = nullptr;
    Vector3 mRelativePos;
    Transform* mTransform = nullptr;
    Transform* mTankTransform = nullptr;
    Transform* mBatteryPointTransform = nullptr;
    Transform* mBatteryTransform = nullptr;

    void startShake(float shakingTime, float shakeAmplitude)
    {
        mShakingTime = shakingTime;
        mShakeAmplitude = shakeAmplitude;
        mElapsedTime = 0;
        mIsShaking = true;
    };

    const float shakeHeightLimit = 3.5f;
    float mShakingTime = 0;
    float mShakeAmplitude = 0;
    float mElapsedTime = 0;
    bool mIsShaking = false;
    
    bool mIsFollowingBullet = false;
    Transform* follingBullet = nullptr;
};
