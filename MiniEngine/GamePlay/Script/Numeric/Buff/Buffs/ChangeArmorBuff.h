#pragma once
#include "GamePlay/Script/Numeric/Buff/BuffBase.h"

class ChangeArmorBuff:public BuffBase
{
public:
    ChangeArmorBuff();
    ArmorType mArmorType = ArmorType::AT_LIGHT;
    BuffBase* clone() const override;
private:
    void tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult) override;
};
