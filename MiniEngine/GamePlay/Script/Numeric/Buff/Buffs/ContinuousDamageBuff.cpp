#include "ContinuousDamageBuff.h"

#include "GamePlay/Script/Numeric/Buff/NumericalCommand.h"

ContinuousDamageBuff::ContinuousDamageBuff()
{
    mId = BuffId::CONTINUOUS_DAMAGE;
    mIsConflictSelf = true;
    mBuffType = BuffType::BT_CONTINUOUS;
    mLifeTime = 5.0f;
    mLeftLifeTime = 5.0f;
    mPriority = 2;
}

BuffBase* ContinuousDamageBuff::clone() const
{
    ContinuousDamageBuff* newBuff = new ContinuousDamageBuff();

    cloneBase(newBuff);

    newBuff->mDamageType = mDamageType;
    newBuff->mDamageValue = mDamageValue;
    return newBuff;
}

void ContinuousDamageBuff::tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult)
{
    //注意computeResult是当前buff挂载物体的属性
    NumericalCommand temp = NumericalCommand::sDamage;
    temp.mDamageType = mDamageType;
    temp.mValue = mDamageValue;
    CommondList.push_back(temp);
}
