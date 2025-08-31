#include "GamePlayUIController.h"

#include "Engine/InputControl/TankinInput.h"
#include "Engine/Utility/GameTime/GameTime.h"
#include "GamePlay/Script/Enemy/AICounter.h"
#include "GamePlay/Script/Player/TankPlayer.h"

REGISTER_COMPONENT(GamePlayUIController, "GamePlayUIController")

void GamePlayUIController::awake()
{
    mStopRoot = mGameObject->getTransform()->getChildWithName("StopRoot")->getGameObject();
    
    mContinueText = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("ContinueText")->getGameObject()->getComponent("TextTGUI"));
    mContinueButton = dynamic_cast<Button*>(
        mGameObject->getTransform()->getChildWithName("ContinueButton")->getGameObject()->getComponent("Button"));
    mBackToMainButton = dynamic_cast<Button*>(
        mGameObject->getTransform()->getChildWithName("BackToMainButton")->getGameObject()->getComponent("Button"));
    mBackToMainText = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("BackToMainText")->getGameObject()->getComponent("TextTGUI"));
    mScoreText = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("ScoreText")->getGameObject()->getComponent("TextTGUI"));
    mTimeText = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("TimeText")->getGameObject()->getComponent("TextTGUI"));

    mContinueButton->setNextButton(mBackToMainButton);
    mContinueButton->setLastButton(mBackToMainButton);
    mBackToMainButton->setNextButton(mContinueButton);
    mBackToMainButton->setLastButton(mContinueButton);

    mContinueButton->addOnClickEvent([this](Button* thisButton)
    {
        GameTime::sResume();
        mStopRoot->deactiveGameObject();
    });

    mContinueButton->addOnUnHoverEvent([](Button* thisButton)
    {
        thisButton->setBlendFactor(1);
    });

    mBackToMainButton->addOnClickEvent([](Button* thisButton)
    {
        GameTime::sResume();
        Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/StartScene.xml");
        AICounter::getInstance()->setLevel(Level::LEVEL_COMMON);
    });

    mBackToMainButton->addOnUnHoverEvent([](Button* thisButton)
    {
        thisButton->setBlendFactor(1);
    });

    Button::sSetCurrentButton(mContinueButton);

    mStopRoot->deactiveGameObject();
    GameTime::sResume();

    mFpsText = dynamic_cast<TextTGUI*>(
        mGameObject->getTransform()->getChildWithName("FPSText")->getGameObject()->getComponent("TextTGUI"));
}

void GamePlayUIController::update()
{
    mFpsText->setText("FPS: " + std::to_string(GameTime::getCurrentFps()));
    
    if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
    {
        mContinueText->setText("继续游戏");
        mBackToMainText->setText("返回主界面");
        mScoreText->setText("分数: " + std::to_string(TankPlayer::sGetInstanse()->mScore));
        mTimeText->setText("时间: " + TankPlayer::sGetInstanse()->getTime());
    }
    else
    {
        mContinueText->setText("Continue");
        mBackToMainText->setText("Back To Main");
        mScoreText->setText("Score: " + std::to_string(TankPlayer::sGetInstanse()->mScore));
        mTimeText->setText("Time: " + TankPlayer::sGetInstanse()->getTime());
    }
    
    if (!mStopRoot->isActive() && TankinInput::sGetInstance()->isKeyDown(Action::OPTION))
    {
        mStopRoot->activeGameObject();
        GameTime::sPause();
        
    }
    else if (mStopRoot->isActive() && TankinInput::sGetInstance()->isKeyDown(Action::OPTION))
    {
        GameTime::sResume();
        mStopRoot->deactiveGameObject();
    }

    //ui启用时的逻辑
    if (mStopRoot->isActive())
    {
        if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_UP))
        {
            Button::sLast();
        }
        else if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_DOWN))
        {
            Button::sNext();
        }

        if (TankinInput::sGetInstance()->isKeyDown(Action::CONFIRM))
        {
            Button::sTrigger();
        }

        static float currentAlpha = 1.0f;
        static float blinkSpeed = 0.1f;
        Button * button = Button::sGetCurrentSelectedButton();
        if (button != nullptr)
        {
            currentAlpha = button->getBlendFactor();
            if (currentAlpha > 0.8f)
            {
                blinkSpeed = -0.02f;
            }
            else if (currentAlpha < 0.3f)
            {
                blinkSpeed = 0.02f;
            }
            currentAlpha += blinkSpeed;
            button->setBlendFactor(currentAlpha);
        }
    }
}
