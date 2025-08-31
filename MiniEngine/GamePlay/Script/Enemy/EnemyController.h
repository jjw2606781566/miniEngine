#pragma once
#include "EnemyState.h"
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/ComponentHeader/TankinRenderComponent.h"
#include "Engine/Component/MonoBehavior.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Component/TGUI/RectTransform.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"



class EnemyController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void onDestory() override;
private:
    RigidBody* mRigidBody = nullptr;
    Transform* mTransform = nullptr;
    RectTransform* mLifeRect = nullptr;
    RectTransform* mLifeBackRect = nullptr;
    Transform* mLifeBarPoint = nullptr;
    Camera* mTankCamera = nullptr;
    Canvas* mCanvas = nullptr;
    Buffable* mBuffable = nullptr;

    //control mesh
    MeshFilter* mBodyMeshFilter = nullptr;
    MeshRenderer* mBodyMeshRenderer = nullptr;
    MeshFilter* mBatteryMeshFilter = nullptr;
    MeshRenderer* mBatteryMeshRenderer = nullptr;

    float mRectWidth = 150;
    float mRectHeight = 10;
    EnemyState mEnemyState = EnemyState::ES_PATROL;
};
