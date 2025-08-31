#include "BuffBase.h"

#include "Engine/common/Exception.h"
#include "Engine/Utility/GameTime/GameTime.h"


void BuffBase::buffUpdate(TpList<NumericalCommand>& CommondList, const NumericalObject& computeResult)
{
    if (mBuffUpdateType == BuffUpdateType::BUT_TIME)
    { //根据时间更新数值
        switch (mBuffType)
        {
        case BuffType::BT_ETERNAL:
            {
                mNextTick -= GameTime::sGetDeltaTime();
                while (mNextTick <= 0)
                {
                    tick(CommondList, computeResult);
                    mNextTick += 1;
                }
                break;
            }
        case BuffType::BT_CONTINUOUS:
            {
                float temp = mLeftLifeTime - GameTime::sGetDeltaTime();
            
                //防止一次gametime时间太长，导致buff剩余时间比当前帧时间小的值大于1秒，会多tick一次buff
                if (temp < -1)
                {
                    mIsDead = true;
                    mNextTick = mLeftLifeTime * -1;
                }
                else
                {
                    mNextTick -= GameTime::sGetDeltaTime();
                }
            
                while (mNextTick <= 0)
                {
                    tick(CommondList, computeResult);
                    mNextTick += 1;
                }
                mLeftLifeTime = temp;
                if (temp < 0)
                {
                    mIsDead = true;
                }
                break;
            }
        case BuffType::BT_INSTANT:
            {
                tick(CommondList, computeResult);
                mIsDead = true;
                break;
            }
        default:
            ASSERT(false, TEXT("Unknown buff type!"));
        }
    }
    else
    {//每帧都更新
        switch (mBuffType)
        {
        case BuffType::BT_ETERNAL:
            tick(CommondList, computeResult);
            break;
        case BuffType::BT_CONTINUOUS:
            {
                mLeftLifeTime -= GameTime::sGetDeltaTime();
                tick(CommondList, computeResult);
                if (mLeftLifeTime <= 0)
                {
                    mIsDead = true;
                }
                break;
            }
        case BuffType::BT_INSTANT:
            {
                tick(CommondList, computeResult);
                mIsDead = true;
                break;
            }
        default:
            ASSERT(false, TEXT("Unknown buff type!"));
        }
    }
}

void BuffBase::cloneBase(BuffBase* newBuff) const
{
    newBuff->mId = mId;
    newBuff->mIsConflictSelf = mIsConflictSelf;
    newBuff->mPriority = mPriority;
    newBuff->mLifeTime = mLifeTime;
    newBuff->mLeftLifeTime = mLeftLifeTime;
    newBuff->mBuffType = mBuffType;
    newBuff->mBuffUpdateType = mBuffUpdateType;
}
