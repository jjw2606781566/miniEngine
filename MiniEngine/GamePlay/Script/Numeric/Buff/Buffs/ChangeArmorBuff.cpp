#include "ChangeArmorBuff.h"

#include "GamePlay/Script/Numeric/Buff/NumericalCommand.h"

ChangeArmorBuff::ChangeArmorBuff()
{
    mId = BuffId::CHANGE_ARMOR;
    mPriority = 1;
    mBuffType = BuffType::BT_ETERNAL;
    mBuffUpdateType = BuffUpdateType::BUT_ALWAYS;
}

BuffBase* ChangeArmorBuff::clone() const
{
    ChangeArmorBuff* newBuff = new ChangeArmorBuff();

    cloneBase(newBuff);

    newBuff->mArmorType = mArmorType;
    return newBuff;
}

void ChangeArmorBuff::tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult)
{
    NumericalCommand changeArmor = NumericalCommand::sSetArmor;
    changeArmor.mArmorType = mArmorType;
    CommondList.push_back(changeArmor);
    NumericalCommand setMoveFactor = NumericalCommand::sSetMoveFactor;
    switch (mArmorType)
    {
        case ArmorType::AT_COMMON:
            setMoveFactor.mValue = 1.5f;
            break;
        case ArmorType::AT_LIGHT:
            setMoveFactor.mValue = 2;
            break;
        case ArmorType::AT_HEAVY:
            setMoveFactor.mValue = 1;
            break;
    }
    CommondList.push_back(setMoveFactor);
}
