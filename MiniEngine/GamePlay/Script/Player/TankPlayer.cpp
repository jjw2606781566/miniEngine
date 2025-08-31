#include "TankPlayer.h"

#include "Engine/common/Exception.h"
#include "GamePlay/Script/Enemy/AICounter.h"

TankPlayer* TankPlayer::sInstance = nullptr;

void TankPlayer::refreshSelf()
{
    mNumericalObject.setAttribute(AttributeType::AT_MAX_LIFE, 200);
    mNumericalObject.setAttribute(AttributeType::AT_CURRENT_LIFE, 200);
    mNumericalObject.setAttribute(AttributeType::AT_ATTACK, 50);
    if (AICounter::getInstance()->getLevel() == Level::LEVEL_COMMON)
    {
        mScore = 0;
        mTime = 0;
    }
    
    AICounter::getInstance()->setZero();
    switch (mTankType)
    {
        case TankType::TT_COMMON:
            {
                mAttackCd = 1;
                mBulletType = BulletType::Common;
                break;
            }
        case TankType::TT_HEAVY:
            {
                mAttackCd = 1.5;
                mBulletType = BulletType::Common;
                break;
            }
        case TankType::TT_LIGHT:
            {
                mAttackCd = 0.75;
                mBulletType = BulletType::Burn;
                break;
            }
        default:
            ASSERT(false, TEXT("Unknown TankType!"))
    }
}
