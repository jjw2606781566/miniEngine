#include "BuffAttacher.h"

#include "Buffable.h"
#include "GamePlay/Script/Player/TankPlayer.h"

REGISTER_COMPONENT(BuffAttacher, "BuffAttacher")

void BuffAttacher::awake()
{
    mRigidBody = dynamic_cast<RigidBody*>(mGameObject->getComponent("RigidBody"));
    ASSERT(mRigidBody, TEXT("BuffAttacher must need a RigidBody!"));
    //注册碰撞添加buff事件
    mRigidBody->addCollisonCallback([this](RigidBody* self, RigidBody* other)
    {
        //DEBUG_FOCUS_PRINT("挂载buff")
        GameObject* otherGo = other->getGameObject();
        for (auto& tag:mTargetTags)
        {
            if (tag == otherGo->getTag())
            {
                Buffable* buffable = dynamic_cast<Buffable*>(otherGo->getComponent("Buffable"));
                ASSERT(buffable, TEXT("BuffAttacher target do not have Buffable component!"));
                buffable->attachBuff(mCarryBuffs);
            }
            if (tag == "Player")
            {
                //表示Player当帧受击
                TankPlayer::sGetInstanse()->mIsAttacked = TankAttackedState::TS_ATTACKING;
            }
        }
    });
}

void BuffAttacher::onDestory()
{
    //析构所有buff
    for (auto& buff: mCarryBuffs)
    {
        SAFE_DELETE_POINTER(buff);
    }
}
