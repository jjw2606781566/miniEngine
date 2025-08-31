#include "BossAIController.h"

REGISTER_COMPONENT(BossAIController, "BossAIController")

void BossAIController::awake()
{
    AIController::awake();
}

void BossAIController::start()
{
    AIController::start();
    mBuffable = dynamic_cast<Buffable*>(mGameObject->getComponent("Buffable"));
    mBossNumerical =  NumericalObject(NumericalType::NT_BOSSTANK);
    mBuffable->setBaseNumericalObject(mBossNumerical);
    setMoveSpeed(bossMaxSpeed);
    setMoveForce(bossMoveForce);
    setRotationSpeed(bossRotationSpeed);
    setPathFindingInterval(bossPathFindingInterval);
}

void BossAIController::update()
{
    AIController::update();
}

void BossAIController::onDestory()
{
    AIController::onDestory();
}
