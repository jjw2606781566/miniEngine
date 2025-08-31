#include "EndUIController.h"

#include "Engine/Application.h"
#include "Engine/InputControl/TankinInput.h"
#include "Engine/Scene/Scene.h"
#include "GamePlay/Script/Enemy/AICounter.h"
#include "GamePlay/Script/Player/RankList.h"
#include "GamePlay/Script/Player/TankPlayer.h"

REGISTER_COMPONENT(EndUIController, "EndUIController")

void EndUIController::awake()
{
    mContinueTextEng = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("ContinueTextEng")->getGameObject()->getComponent("TextTGUI"));
    mCai = dynamic_cast<ImageTGUI*>(
        mGameObject->getTransform()->getChildWithName("Cai")->getGameObject()->getComponent("ImageTGUI"));
    mCai->setBlendFactor(1);
    mContinueTextEng->getGameObject()->deactiveGameObject();
    RankList::sGetInstance()->addRank(TankPlayer::sGetInstanse()->mTime, TankPlayer::sGetInstanse()->mScore);
    //保存存档数据
    RankList::sGetInstance()->save();
    //切到这个场景时，就一定是需要从头开始打了
    AICounter::getInstance()->setLevel(Level::LEVEL_COMMON);
}


void EndUIController::update()
{
    if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
    {
#ifdef WIN32
        mContinueTextEng->setText("按回车键以继续游戏");
#endif
#ifdef ORBIS
        mContinueTextEng->setText("按X键以继续游戏");
#endif
    }
    else
    {
#ifdef WIN32
        mContinueTextEng->setText("Press Enter to continue");
#endif
#ifdef ORBIS
        mContinueTextEng->setText("Press X to continue");
#endif
    }
    
    if (mIsFirstShow)
    {
        firstUpdateAnimation();
    }
    else if (TankinInput::sGetInstance()->isKeyDown(Action::CONFIRM))
    {
        Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/StartScene.xml");
    }

    static float currentAlpha = 1.0f;
    static float blinkSpeed = 0.02f;
    currentAlpha = mContinueTextEng->getBlendFactor();
    if (currentAlpha > 1)
    {
        blinkSpeed = -0.02f;
    }
    else if (currentAlpha < 0)
    {
        blinkSpeed = 0.02f;
    }
    currentAlpha += blinkSpeed;
    mContinueTextEng->setBlendFactor(currentAlpha);
}

void EndUIController::firstUpdateAnimation()
{
    static float alphaSpeed = -0.01f;
    if (mCai->getBlendFactor() > 0.01)
    {
        mCai->setBlendFactor(mCai->getBlendFactor()+alphaSpeed);
    }
    else
    {
        mContinueTextEng->getGameObject()->activeGameObject();
        mIsFirstShow = false;
    }
    
}
