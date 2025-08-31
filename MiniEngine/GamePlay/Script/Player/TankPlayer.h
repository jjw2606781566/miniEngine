#pragma once
#pragma execution_character_set("utf-8")
#include "GamePlay/Script/Numeric/Buff/NumericalObject.h"
#include "GamePlay/Script/Shell/ShellFactory.h"

class TankCameraController;

enum class TankPlayerState : char
{
    TP_IDLE,
    TP_MOVE,
    TP_AIMING,
    TP_ATTACKING,
    TP_DEAD,
};

enum class TankType : char
{
    TT_COMMON, //最普通的坦克，只有最平凡的普通攻击，攻击速度1秒一发
    TT_LIGHT, //轻型坦克，配备燃烧弹，移动速度较快，攻击速度快，但燃烧buff只能同时存在一个
    TT_HEAVY, //重型坦克，配备普通子弹，攻击速度慢，移动速度较慢，但是重甲，并且可以上下调整炮管
    TT_COUNT
};

//坦克受击状态
enum class TankAttackedState : char{
    TS_NONE, //不受击
    TS_ATTACKING, //本帧受击
    TS_ATTACKED //之前受击
};

enum class LanguageType : char{
    LT_ENGLISH,
    LT_CHINESE,
    LT_COUNT
};

class TankPlayer
{
public:
    static TankPlayer* sGetInstanse()
    {
        if (sInstance == nullptr)
            sInstance = new TankPlayer();
        return sInstance;
    }
    TankPlayerState getTankPlayerState() const {return mTankPlayerState;}
    void changeTankPlayerState(TankPlayerState state) {mTankPlayerState = state;}
    TankType getTankType() const{return mTankType;}
    void setTankType(TankType type) {mTankType = type;}
    BulletType getBulletType()const {return mBulletType;}
    float getAttackCd() const{return mAttackCd;}
    void refreshSelf(); //根据当前坦克种类刷新自己
    void changeLanguage()
    {
        if (languageType == LanguageType::LT_ENGLISH)
            languageType = LanguageType::LT_CHINESE;
        else
        {
            languageType = LanguageType::LT_ENGLISH;
        }
    }
    LanguageType getLanguageType() const {return languageType;}

    TankAttackedState mIsAttacked = TankAttackedState::TS_NONE;
    NumericalObject mNumericalObject;
    TankCameraController* tankCameraController = nullptr;

    //当前积分
    int mScore = 0;
    inline TpString getTime() const
    {
        TpString finalResult;
        int minutes = static_cast<int>(mTime)/60;
        int second = static_cast<int>(mTime) - minutes*60;
        int millisecond = static_cast<int>(mTime*100 - static_cast<int>(mTime)*100);
        return std::to_string(minutes) + ": " + std::to_string(second) + ": " + std::to_string(millisecond);
    }
    //当前时间
    float mTime = 0;
private:
    TankPlayer():mNumericalObject(NumericalType::NT_TANK){}
    static TankPlayer* sInstance;
    TankPlayerState mTankPlayerState = TankPlayerState::TP_IDLE;
    
    TankType mTankType = TankType::TT_HEAVY;
    BulletType mBulletType = BulletType::Common;
    
    LanguageType languageType = LanguageType::LT_ENGLISH;
    float mAttackCd = 0;

    
};
