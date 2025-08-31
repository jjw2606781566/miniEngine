#include "ShellController.h"

#include "Engine/Component/GameObject.h"
#include "Engine/Physical/Shape/BoxShape.h"
#include "Engine/Utility/GameTime/GameTime.h"
#include "GamePlay/Script/Numeric/Component/Buffable.h"
#include "GamePlay/Script/Player/TankCameraController.h"
#include "GamePlay/Script/Player/TankPlayer.h"

REGISTER_COMPONENT(ShellController, "ShellController")

void ShellController::awake()
{
    mTransform = mGameObject->getTransform();
    mRigidBody = dynamic_cast<RigidBody*>(mGameObject->getComponent("RigidBody"));
}

void ShellController::start()
{
    mRigidBody->addCollisonCallback([this](RigidBody* self, RigidBody* other)
    {
        if (other->getGameObject()->getTag() == "Aming" || other->getGameObject()->getTag() == "Shell")
        {
            return;
        }
        GameObject* selfObject = self->getGameObject();
        mParticleSystem->stopGenerate();
        mDelayDestruction->startTiming();
        mBoomParticleSystem->getGameObject()->getTransform()->setWorldPosition(mTransform->getWorldPosition());
        mBoomParticleSystem->startGenerate();
        mBoomDelayDestruction->startTiming();
        GameObjectFactory::sDestroyGameObject(selfObject);
    });
    
    BoxShape* tankShape = new BoxShape({0.5f, 0.5f, 0.5f});
    mRigidBody->setShape(tankShape);
}

void ShellController::update()
{
    mParticleSystem->getGameObject()->getTransform()->setWorldPosition(mTransform->getWorldPosition());
}

void ShellController::onDestory()
{
    if (TankPlayer::sGetInstanse()->tankCameraController->getBulletTransform() == mGameObject->getTransform())
    {
        TankPlayer::sGetInstanse()->tankCameraController->unFollowBullet();
        GameTime::sSetTimeScale(1);
    }
}
