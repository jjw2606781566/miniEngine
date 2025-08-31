#pragma once
#include "AIController.h"

class BossAIController : public AIController
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void onDestory() override;
private:
    Buffable* mBuffable = nullptr;
    NumericalObject mBossNumerical;
    const float bossMaxSpeed = 8.0f;
    const float bossMoveForce = 4.0f;
    const float bossRotationSpeed = 1.0f;
    const float bossPathFindingInterval = 0.50f;
};
