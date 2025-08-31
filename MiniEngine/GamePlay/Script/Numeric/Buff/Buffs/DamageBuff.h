#pragma once
#include "GamePlay/Script/Numeric/Buff/BuffBase.h"

class DamageBuff : public BuffBase
{
public:
    DamageBuff();
    DamageType mDamageType = DamageType::DT_COMMON;
    float mDamageValue = 0;
    BuffBase* clone() const override;
private:
    void tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult) override;
};
