#pragma once

#include "AmingLine.h"
#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/Particle/ParticleSystem.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Component/RenderComponent/MeshFilter.h"
#include "Engine/Component/RenderComponent/MeshRenderer.h"
#include "Engine/Component/TGUI/RectTransform.h"
#include "Engine/Component/TGUI/TextTGUI.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"

class TankController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void fixedUpdate() override;
private:
    void breatheHeal();
    Transform* mTankTransform = nullptr;
    Transform* mTankBodyTransform = nullptr;
    RigidBody* mTankRigidBody = nullptr;
    Transform* mBatteryTransform = nullptr;
    Transform* mBatteryPointTransform = nullptr;
    Transform* mGunTransform = nullptr;
    RectTransform* mLifeRect = nullptr;
    TextTGUI* mTextCd = nullptr;
    
    AudioSource* mAudioSource = nullptr;
    
    Buffable* mBuffable = nullptr;

    void setAndChangeTankModel();
    
    MeshFilter* mBodyMeshFilter = nullptr;
    MeshRenderer* mBodyMeshRenderer = nullptr;
    MeshFilter* mBatteryMeshFilter = nullptr;
    MeshRenderer* mBatteryMeshRenderer = nullptr;
    MeshFilter* mGunMeshFilter = nullptr;
    MeshRenderer* mGunMeshRenderer = nullptr;
    MeshFilter* mTrackMeshFilter = nullptr;
    MeshRenderer* mTrackMeshRenderer = nullptr;
    
    float mLifeWidth = 600;

    ParticleSystem* mParticleSystem = nullptr;

    float mAttackCd = 0;

    //画线
    AmingLine* mAmingLine = nullptr;
};
