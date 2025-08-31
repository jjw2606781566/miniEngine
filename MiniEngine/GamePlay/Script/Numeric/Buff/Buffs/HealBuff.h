#pragma once
#include "GamePlay/Script/Numeric/Buff/BuffBase.h"

class HealBuff : public BuffBase
{
public:
    HealBuff();
    float mHealValue = 1;
    BuffBase* clone() const override;

protected:
    void tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult) override;
    
};
