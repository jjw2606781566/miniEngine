#pragma once
#include <functional>
#include <intsafe.h>

#include "NumericalObject.h"
#include "Engine/Memory/TankinMemory.h"

enum class BuffType : char
{
    BT_INSTANT,
    BT_CONTINUOUS,
    BT_ETERNAL
};

enum class BuffUpdateType : char{
    BUT_TIME,
    BUT_ALWAYS
};

enum class BuffId : char{
    BASE,
    CHANGE_ARMOR,
    CONTINUOUS_DAMAGE,
    DAMAGE,
    HEAL,
    COUNT
};

class BuffBase
{
public:
    BuffBase() = default;
    virtual ~BuffBase() = default;
    void setLifeTime(float lifeTime){mLifeTime = lifeTime; mLeftLifeTime = lifeTime;}
    bool isDead() const {return mIsDead;}
    UINT32 getPriority() const {return mPriority;}
    BuffId getId() const {return mId;}
    bool isConflictSelf() const{return mIsConflictSelf;}
    void buffUpdate(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult);
    void refreshSelf() {mLeftLifeTime = mLifeTime;}
    virtual BuffBase* clone() const = 0;

protected:
    BuffId mId = BuffId::BASE;
    bool mIsConflictSelf = false; //是否和自己冲突，如果冲突则刷新buff剩余时间
    //克隆父buff的属性
    virtual void cloneBase(BuffBase* newBuff) const;
    //整体时间
    float mLifeTime = 0;
    //当前剩余时间
    float mLeftLifeTime = 0;
    //Buff持续类型
    BuffType mBuffType = BuffType::BT_INSTANT;
    //Buff更新类型
    BuffUpdateType mBuffUpdateType = BuffUpdateType::BUT_TIME;
    //更新优先级
    UINT32 mPriority = 1;
    //之后的继承需要向目标中添加数值命令
    virtual void tick(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult) = 0;
    //距离下次触发更新还需要几秒，默认1秒触发一次buff
    float mNextTick = 1;
    //是否是消亡buff
    bool mIsDead = false;
};
