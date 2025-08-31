#include "NumericalCommand.h"

const NumericalCommand NumericalCommand::sHeal = {
    NumericalCommandType::NCT_HEAL,
    AttributeType::AT_CURRENT_LIFE,
    0,
    ArmorType::AT_COMMON,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
}; 

const NumericalCommand NumericalCommand::sDamage = {
    NumericalCommandType::NCT_DAMAGE,
    AttributeType::AT_CURRENT_LIFE,
    0,
    ArmorType::AT_COMMON,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
};

const NumericalCommand NumericalCommand::sChangeAttribute = {
    NumericalCommandType::NCT_ATTRIBUTE,
    AttributeType::AT_ATTACK,
    0,
    ArmorType::AT_COMMON,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
}; 
const NumericalCommand NumericalCommand::sTempSetArmor = {
    NumericalCommandType::NCT_SET_ARMOR,
    AttributeType::AT_CURRENT_LIFE,
    0,
    ArmorType::AT_LIGHT,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
};
const NumericalCommand NumericalCommand::sSetMoveFactor{
    NumericalCommandType::NCT_SET_MOVE_FACTOR,
    AttributeType::AT_MOVE_FACTOR,
    1.0,
    ArmorType::AT_LIGHT,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
};
const NumericalCommand NumericalCommand::sSetArmor = {
    NumericalCommandType::NCT_SET_ARMOR,
    AttributeType::AT_CURRENT_LIFE,
    0,
    ArmorType::AT_LIGHT,
    DamageType::DT_COMMON,
    EumericalStateType::EST_COMMON
};

const NumericalCommand NumericalCommand::sDaddy = {
    NumericalCommandType::NCT_SET_STATE,
    AttributeType::AT_CURRENT_LIFE,
    0,
    ArmorType::AT_COMMON,
    DamageType::DT_COMMON,
    EumericalStateType::EST_DADDY
}; 