#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "GamePlay/Script/Numeric/Buff/NumericalObject.h"

//普通物体
class Destroyable : public MonoBehavior
{
public:
private:
    NumericalObject mNumeric;
};
