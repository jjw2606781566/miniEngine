#include "NumericalObject.h"
#include "NumericalCommand.h"

void NumericalObject::executeCommondList(TpList<NumericalCommand>& CommondList, NumericalObject& computeResult)
{
    //先将原始数值复制过去,除了当前血量
    computeResult = *this;

    //挨个更新数值命令
    for (auto& command : CommondList)
    {
        switch (command.mCommandType)
        {
            case NumericalCommandType::NCT_ATTRIBUTE:
                computeResult.mAttributeMap[command.mAttributeType] += command.mValue;
                break;
            case NumericalCommandType::NCT_DAMAGE:
                if (computeResult.mStateType == EumericalStateType::EST_DADDY)
                { //无敌状态不需要计算伤害了
                    break;
                }
                if (computeResult.mArmorType == ArmorType::AT_HEAVY)
                {
                    //重甲对于其他子弹类型有减伤，而对于穿透则会增伤
                   if (command.mDamageType == DamageType::DT_PIERCING) 
                   {
                       computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] -= command.mValue * 1.2f;
                   }
                   else
                   {
                       computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] -= command.mValue * 0.7f;
                   }
                }
                else if (computeResult.mArmorType == ArmorType::AT_LIGHT)
                {
                    //轻甲对于其他子弹类型会有轻微增伤，而对于爆炸子弹则会大量增伤
                    if (command.mDamageType == DamageType::DT_EXPLOSIVE)
                    {
                        computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] -= command.mValue * 1.5f;
                    }
                    else
                    {
                        computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] -= command.mValue * 1.2f;
                    }
                }
                else
                {
                    computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] -= command.mValue;
                }
                break;
            case NumericalCommandType::NCT_HEAL:
                //回血
                computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] += command.mValue;
                break;
            case NumericalCommandType::NCT_SET_ARMOR:
                computeResult.mArmorType = command.mArmorType;
                break;
            case NumericalCommandType::NCT_SET_STATE:
                //设置状态
                computeResult.mStateType = command.mStateType;
                break;
            case NumericalCommandType::NCT_SET_MOVE_FACTOR:
                computeResult.mAttributeMap[AttributeType::AT_MOVE_FACTOR] = command.mValue;
                break;
        }
    }

    //结束后血量修正，防止当前血量大于最大血量，防止血量为负
    float currentLife = computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE];
    float maxLife = computeResult.mAttributeMap[AttributeType::AT_MAX_LIFE];
    currentLife = currentLife < maxLife ? currentLife : maxLife;
    currentLife = currentLife > 0 ? currentLife : 0;
    computeResult.mAttributeMap[AttributeType::AT_CURRENT_LIFE] = currentLife;
    //更新记录的数值，因为每次处理命令队列都会先把这个数值复制过去
    mAttributeMap[AttributeType::AT_CURRENT_LIFE] = currentLife;
    //结束时清空命令队列
    CommondList.clear();
}
