#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Memory/TankinMemory.h"
#include "GamePlay/Script/Numeric/Buff/BuffBase.h"

class BuffAttacher : public MonoBehavior
{
public:
    void awake() override;
    void onDestory() override;
    void addBuff(BuffBase* buff)
    {
        mCarryBuffs.push_back(buff);
    }
    void addTargetTag(const TpString& tag)
    {
        mTargetTags.push_back(tag);
    }
    float damage;
private:
    TpList<BuffBase*> mCarryBuffs;
    TpList<TpString> mTargetTags;
    RigidBody* mRigidBody = nullptr;
};
