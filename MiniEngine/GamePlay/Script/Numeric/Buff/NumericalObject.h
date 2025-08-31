#pragma once
#include "Engine/common/Exception.h"
#include "Engine/Memory/TankinMemory.h"

struct NumericalCommand;

//不需要存储ArmorType，这个只需要buff更新时记录一下就行
enum class ArmorType : char
{
    AT_COMMON,
    AT_HEAVY,
    AT_LIGHT
};

//计算伤害时，判断伤害类型
enum class DamageType : char
{
    DT_COMMON,
    DT_PIERCING,
    DT_EXPLOSIVE
};

//属性类型
enum class AttributeType : char{
    AT_MAX_LIFE,
    AT_CURRENT_LIFE,
    AT_ATTACK,
    AT_MOVE_FACTOR
};

//状态类型
enum class EumericalStateType : char{
    EST_COMMON,
    EST_DADDY, //whos your daddy?
};

//数值类型，用于快速创建新的数值
enum class NumericalType : char{
    NT_BOSSTANK,
    NT_TANK,
    NT_COUNT
};

struct NumericalObject
{
    NumericalObject()
    {
        mAttributeMap[AttributeType::AT_MAX_LIFE] = 100;
        mAttributeMap[AttributeType::AT_CURRENT_LIFE] = 100;
    }
    NumericalObject(NumericalType type)
    {
        mAttributeMap[AttributeType::AT_MAX_LIFE] = 100;
        mAttributeMap[AttributeType::AT_CURRENT_LIFE] = 100;
        switch (type)
        {
        case NumericalType::NT_BOSSTANK:
            mAttributeMap[AttributeType::AT_MAX_LIFE] = 1000;
            mAttributeMap[AttributeType::AT_CURRENT_LIFE] = 1000;
            mAttributeMap[AttributeType::AT_ATTACK] = 20;
            mAttributeMap[AttributeType::AT_MOVE_FACTOR] = 1;
        case NumericalType::NT_TANK:
                mAttributeMap[AttributeType::AT_ATTACK] = 10;
                mAttributeMap[AttributeType::AT_MOVE_FACTOR] = 1;
                break;
            default:
                ASSERT(false, TEXT("Unknown numerical type!"));
        }
    }
    bool isDie() const
    {
        return mAttributeMap.at(AttributeType::AT_CURRENT_LIFE) <= 0;
    }
    
    float getLifePercentage() const
    {
        return mAttributeMap.at(AttributeType::AT_CURRENT_LIFE)
            /mAttributeMap.at(AttributeType::AT_MAX_LIFE);
    }
    float getAttribute(AttributeType type) const
    {
        return mAttributeMap.at(type);
    }
    void setAttribute(AttributeType type, float value)
    {
        mAttributeMap.at(type) = value;
    }
    //执行数值命令列表，最终结果存储在computeResult中
    void executeCommondList(TpList<NumericalCommand>& CommondList, NumericalObject& computeResult);
private:
    TpUnorderedMap<AttributeType, float> mAttributeMap;
    ArmorType mArmorType = ArmorType::AT_COMMON;
    EumericalStateType mStateType = EumericalStateType::EST_COMMON;
};
