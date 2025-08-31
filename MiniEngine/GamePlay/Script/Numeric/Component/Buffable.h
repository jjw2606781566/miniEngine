#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "../Buff/NumericalObject.h"
#include "../Buff/NumericalCommand.h"
#include "GamePlay/Script/Numeric/Buff/BuffBase.h"

class Buffable : public MonoBehavior
{
public:
    void awake() override;
    void update() override;
    void onDestory() override;
    bool isDie() const
    {
        return mIsDie;
    }
    float getLifePecentage( ) const {return mResultNumerical.getLifePercentage();}
    float getCurrentLife() const {return mResultNumerical.getAttribute(AttributeType::AT_CURRENT_LIFE);}
    void attachBuff(const TpList<BuffBase*>& buffList);
    void setBaseNumericalObject(const NumericalObject& numerical)
    {
        mBaseNumerical = numerical;
    }
    NumericalObject* getResultNumerical()
    {
        return &mResultNumerical;
    }
private:
    NumericalObject mBaseNumerical;
    NumericalObject mResultNumerical;
    TpList<NumericalCommand> mCommandLists;
    TpMultiMap<UINT32, BuffBase*> mBuffMap;
    TpUnorderedMap<BuffId, BuffBase*> mBuffIdMap;//记录同样ID的buff有几个
    bool mIsDie = false;
};
