#include "HealBuff.h"

#include "GamePlay/Script/Numeric/Buff/NumericalCommand.h"

HealBuff::HealBuff()
{
    mId = BuffId::HEAL;
    mPriority = 2;
    
}

BuffBase* HealBuff::clone() const
{
    HealBuff* newBuff = new HealBuff();
    
    //先克隆一下基类的属性
    cloneBase(newBuff);

    //再克隆自己的属性
    newBuff->mHealValue = mHealValue;
    return newBuff;
}

void HealBuff::tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult)
{
    NumericalCommand temp = NumericalCommand::sHeal;
    temp.mValue = mHealValue;
    CommondList.push_back(temp);
}
