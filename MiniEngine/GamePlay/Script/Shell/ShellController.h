#pragma once
#include "Engine/Component/MonoBehavior.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/Particle/ParticleSystem.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "GamePlay/Script/Utility/DelayDestruction.h"

class ShellController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void onDestory() override;
    ParticleSystem* mParticleSystem = nullptr;
    DelayDestruction* mDelayDestruction = nullptr;
    ParticleSystem* mBoomParticleSystem = nullptr;
    DelayDestruction* mBoomDelayDestruction = nullptr;
private:
    Transform* mTransform = nullptr;
    RigidBody* mRigidBody = nullptr;
};
