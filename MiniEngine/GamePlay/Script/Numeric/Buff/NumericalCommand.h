#pragma once
#include "NumericalObject.h"

//命令的类型
enum class NumericalCommandType : char
{
    NCT_ATTRIBUTE, //更新某个属性的值，后续跟属性类型和调整值
    NCT_DAMAGE, //造成伤害
    NCT_HEAL, //恢复血量
    NCT_SET_ARMOR,
    NCT_SET_STATE, //主要用于设置无敌，带这种命令的buff最好是最高优先级
    NCT_SET_MOVE_FACTOR, //用于设置移动因子
};

struct NumericalCommand
{
    //命令类型，决定如何解析这个命令
    NumericalCommandType mCommandType;

    //解析命令所需要的对应参数
    AttributeType mAttributeType = AttributeType::AT_MAX_LIFE;
    float mValue;
    ArmorType mArmorType = ArmorType::AT_COMMON;
    DamageType mDamageType = DamageType::DT_COMMON;
    EumericalStateType mStateType = EumericalStateType::EST_COMMON;

    //预定义命令
    static const NumericalCommand sHeal; //治疗命令，填写治疗量
    static const NumericalCommand sDamage; //伤害命令，填写伤害量和伤害类型
    static const NumericalCommand sChangeAttribute; //修改属性命令，填写属性类型和更新值
    static const NumericalCommand sSetArmor; //永久修改护甲类型
    static const NumericalCommand sSetMoveFactor;
    static const NumericalCommand sTempSetArmor; //临时设置护甲命令，用于一些特殊卡牌
    static const NumericalCommand sDaddy; //设置无敌命令
};
