#include "DamageBuff.h"

#include "GamePlay/Script/Numeric/Buff/NumericalCommand.h"


DamageBuff::DamageBuff()
{
    mId = BuffId::DAMAGE;
    mPriority = 2;
}

BuffBase* DamageBuff::clone() const
{
    DamageBuff* newBuff = new DamageBuff();
    
    //先克隆一下基类的属性
    cloneBase(newBuff);

    //再克隆自己的属性
    newBuff->mDamageType = mDamageType;
    newBuff->mDamageValue = mDamageValue;
    return newBuff;
}

void DamageBuff::tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult)
{
    //注意computeResult是当前buff挂载物体的属性
    NumericalCommand temp = NumericalCommand::sDamage;
    temp.mDamageType = mDamageType;
    temp.mValue = mDamageValue;
    CommondList.push_back(temp);
}
