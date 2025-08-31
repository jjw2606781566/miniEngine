#include "UIController.h"

#include "Engine/Application.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/TGUI/Button.h"
#include "Engine/Component/TGUI/RectTransform.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/InputControl/TankinInput.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Utility/GameTime/GameTime.h"
#include "Engine/Window/Frame.h"
#include "GamePlay/Script/Player/RankList.h"
#include "GamePlay/Script/Player/TankPlayer.h"

REGISTER_COMPONENT(UIController, "UIController")

void UIController::CreateButton()
{
    //读取存档数据
    RankList::sGetInstance()->load();
    
    //Button
    mButtonNewGame = dynamic_cast<Button*>(
        mCanvas->getTransform()->getChildWithName("NewGameButton")->getGameObject()->getComponent("Button"));
    mButtonChangeLanguage = dynamic_cast<Button*>(
        mCanvas->getTransform()->getChildWithName("ChangeLanguageButton")->getGameObject()->getComponent("Button"));
    mButtonShowInput = dynamic_cast<Button*>(
        mCanvas->getTransform()->getChildWithName("ShowInputButton")->getGameObject()->getComponent("Button"));
    mInput = dynamic_cast<ImageTGUI*>(
        mCanvas->getTransform()->getChildWithName("Input")->getGameObject()->getComponent("ImageTGUI"));
    mButtonRankList = dynamic_cast<Button*>(
        mCanvas->getTransform()->getChildWithName("RankListButton")->getGameObject()->getComponent("Button"));

    //获取排行榜
    mRankListTransform = mCanvas->getTransform()->getChildWithName("RankList");
    mRankText = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("RankText")->getGameObject()->getComponent("TextTGUI"));
    mScoreText = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("ScoreText")->getGameObject()->getComponent("TextTGUI"));
    mTimeText = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("TimeText")->getGameObject()->getComponent("TextTGUI"));
    mRankTextRanks = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("RankTextRanks")->getGameObject()->getComponent("TextTGUI"));
    mScoreTextScores = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("ScoreTextScores")->getGameObject()->getComponent("TextTGUI"));
    mTimeTextTimes = dynamic_cast<TextTGUI*>(
        mRankListTransform->getChildWithName("TimeTextTimes")->getGameObject()->getComponent("TextTGUI"));
    
    
    //先隐藏button
    mButtonNewGame->getGameObject()->deactiveGameObject();
    mButtonChangeLanguage->getGameObject()->deactiveGameObject();
    mButtonShowInput->getGameObject()->deactiveGameObject();
    mButtonRankList->getGameObject()->deactiveGameObject();
    
    mButtonNewGame->addOnUnHoverEvent([](Button* thisButton){
        thisButton->setBlendFactor(0);
    });
    mButtonNewGame->addOnClickEvent([this](Button* thisButton){
        printf("%s OnClick!\n", thisButton->getGameObjectName().c_str() ); fflush(stdout);
        mSelectingTank = true;
        mButtonNewGame->getGameObject()->deactiveGameObject();
        mButtonChangeLanguage->getGameObject()->deactiveGameObject();
        mButtonShowInput->getGameObject()->deactiveGameObject();
        mButtonRankList->getGameObject()->deactiveGameObject();
        mTitle->getGameObject()->deactiveGameObject();
        current = static_cast<char>(TankPlayer::sGetInstanse()->getTankType());
        tankParticles[static_cast<char>(TankPlayer::sGetInstanse()->getTankType())]->startGenerate();
    });
    
    mButtonChangeLanguage->addOnUnHoverEvent([](Button* thisButton){
        thisButton->setBlendFactor(0);
    });
    mButtonChangeLanguage->addOnClickEvent([](Button* thisButton){
        TankPlayer::sGetInstanse()->changeLanguage();
    });

    mButtonShowInput->addOnUnHoverEvent([](Button* thisButton)
    {
       thisButton->setBlendFactor(0); 
    });
    mButtonShowInput->addOnClickEvent([this](Button* thisButton)
    {
        mShowingInput = true;
        mInput->getGameObject()->activeGameObject();
    });

    mButtonRankList->addOnUnHoverEvent([this](Button* thisButton)
    {
        thisButton->setBlendFactor(0);
    });
    mButtonRankList->addOnClickEvent([this](Button* thisButton)
    {
        mShowingRankList = true;
        mRankListTransform->getGameObject()->activeGameObject();
        mScoreTextScores->setText(RankList::sGetInstance()->getRankListScoreString());
        mTimeTextTimes->setText(RankList::sGetInstance()->getRankListTimeString());
    });

    mButtonNewGame->setNextButton(mButtonChangeLanguage);
    mButtonNewGame->setLastButton(mButtonRankList);
    
    mButtonChangeLanguage->setNextButton(mButtonShowInput);
    mButtonChangeLanguage->setLastButton(mButtonNewGame);
    
    mButtonShowInput->setNextButton(mButtonRankList);
    mButtonShowInput->setLastButton(mButtonChangeLanguage);
    
    mButtonRankList->setNextButton(mButtonNewGame);
    mButtonRankList->setLastButton(mButtonShowInput);

    mButtonNewGame->setBlendFactor(0);
    mButtonChangeLanguage->setBlendFactor(0);
    mButtonShowInput->setBlendFactor(0);
    mButtonRankList->setBlendFactor(0);

    Button::sSetCurrentButton(mButtonNewGame);

    mTitle = dynamic_cast<ImageTGUI*>(
        mCanvas->getTransform()->getChildWithName("Title")->getGameObject()->getComponent("ImageTGUI"));
    mTitle->setBlendFactor(1);
}

void UIController::firstShowAnimation()
{
    static float alphaSpeed = -0.01f;
    static float moveSpeed = 11;
    if (mTitle->getBlendFactor() > 0.01f)
    {
        mTitle->setBlendFactor(mTitle->getBlendFactor() + alphaSpeed);
        mTankRoot1->setWorldPosition(mTankRoot1->getWorldPosition() + mTankRoot1->getForward()*moveSpeed*GameTime::sGetDeltaTime());
        mTankRoot2->setWorldPosition(mTankRoot2->getWorldPosition() + mTankRoot2->getForward()*moveSpeed*GameTime::sGetDeltaTime());
        mTankRoot3->setWorldPosition(mTankRoot3->getWorldPosition() + mTankRoot3->getForward()*moveSpeed*GameTime::sGetDeltaTime());
    }
    else
    {
        mButtonNewGame->getGameObject()->activeGameObject();
        mButtonChangeLanguage->getGameObject()->activeGameObject();
        mButtonShowInput->getGameObject()->activeGameObject();
        mButtonRankList->getGameObject()->activeGameObject();
        mFirstShow = false;
        mCameraPos[0] = {mTankRoot1->getWorldPosition().v.x, 359.29, -994.58};
        mCameraPos[1] = {mTankRoot2->getWorldPosition().v.x, 359.29, -994.58};
        mCameraPos[2] = {mTankRoot3->getWorldPosition().v.x, 359.29, -994.58};
    }
}

void UIController::awake()
{
    mTransform = mGameObject->getTransform();
    mCanvas = dynamic_cast<Canvas*>(mGameObject->getComponent("Canvas"));

    CreateButton();
    //获得坦克
    mTankRoot1 = Transform::sFindGameObject("TankRoot");
    mTankRoot2 = Transform::sFindGameObject("TankRoot2");
    mTankRoot3 = Transform::sFindGameObject("TankRoot3");

    //选坦克部分
    mSelectingTank = false;
    mCamera = Transform::sFindGameObject("tankCamera");
    mCameraPos.resize(4);
    mCameraPos[0] = {mTankRoot1->getWorldPosition().v.x, 359.29, -994.58};
    mCameraPos[1] = {mTankRoot2->getWorldPosition().v.x, 359.29, -994.58};
    mCameraPos[2] = {mTankRoot3->getWorldPosition().v.x, 359.29, -994.58};
    mCameraPos[3] = {1000, 360, -1000};
    mSelectTankText = dynamic_cast<TextTGUI*>(Transform::sFindGameObject("SelectTankText")->getGameObject()
        ->getComponent("TextTGUI"));

    //獲得坦克的粒子系統
    tankParticles.push_back(dynamic_cast<ParticleSystem*>(
        mTankRoot1->getGameObject()->getComponent("ParticleSystem")));
    tankParticles.push_back(dynamic_cast<ParticleSystem*>(
        mTankRoot2->getGameObject()->getComponent("ParticleSystem")));
    tankParticles.push_back(dynamic_cast<ParticleSystem*>(
        mTankRoot3->getGameObject()->getComponent("ParticleSystem")));

    for (auto& particle:tankParticles)
    {
        particle->stopGenerate();
    }

    //设置bgm
    mAudioSource = dynamic_cast<AudioSource*>(mGameObject->getComponent("AudioSource"));
    mAudioSource->addAuidioClip("DarkSouls3", FileManager::sGetLoadedBolbFile<AudioClip*>("DarkSouls3"));

    mFpsText = dynamic_cast<TextTGUI*>(
        mTransform->getChildWithName("FPSText")->getGameObject()->getComponent("TextTGUI"));
}

void UIController::start()
{
    mAudioSource->playLoop("DarkSouls3");
}

void UIController::update()
{
    if (mShowingInput)
    {
        if (TankinInput::sGetInstance()->isKeyDown(Action::CANCEL))
        {
            mShowingInput = false;
            mInput->getGameObject()->deactiveGameObject();
        }
        return;
    }

    if (mShowingRankList)
    {
        if (TankinInput::sGetInstance()->isKeyDown(Action::CANCEL))
        {
            mShowingRankList = false;
            mRankListTransform->getGameObject()->deactiveGameObject();
        }
    }
    
    mFpsText->setText("FPS: " + std::to_string(GameTime::getCurrentFps()));
    
    //localization
    if (TankPlayer::sGetInstanse()->getLanguageType() == LanguageType::LT_CHINESE)
    {
        mButtonChangeLanguage->setTexture("ChangeLanguage_en");
        mButtonNewGame->setTexture("NewGame_ch");
        mSelectTankText->setText("按左右键切换坦克");
        mButtonShowInput->setTexture("Tutorial_ch");
        mButtonRankList->setTexture("RankListButton_ch");
        mRankText->setText("排名");
        mScoreText->setText("分数");
        mTimeText->setText("时间");
#ifdef WIN32
        mInput->setTexture("PC_ch");
#endif
#ifdef ORBIS
        mInput->setTexture("PS4_ch");
#endif
    }
    else
    {
        mButtonChangeLanguage->setTexture("ChangeLanguage_ch");
        mButtonNewGame->setTexture("NewGame_en");
        mSelectTankText->setText("Press left or right keys \nto switch tanks");
        mButtonShowInput->setTexture("Tutorial_en");
        mButtonRankList->setTexture("RankListButton_en");
        mRankText->setText("Rank");
        mScoreText->setText("Score");
        mTimeText->setText("Time");
#ifdef WIN32
        mInput->setTexture("PC_en");
#endif
#ifdef ORBIS
        mInput->setTexture("PS4_en");
#endif
    }
    
    if (mSelectingTank)
    {
        //选择坦克
        static float lerpFactor = 0.05f;
        static float currentLerpStep = 0;
        static const float LerpStep = 0.03;
        static Vector3 LerpStart;
        
        Vector3 targetPos = mCameraPos[current];
        Vector3 currentPos = mCamera->getWorldPosition();
        if (current != 3)
        {
            mCamera->setWorldPosition(Vector3::Lerp(currentPos, targetPos, lerpFactor));
        }
        else
        {
            currentLerpStep += LerpStep;
            mCamera->setWorldPosition(Vector3::Lerp(LerpStart, targetPos, currentLerpStep));
        }
        
        if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_RIGHT) && current != 3)
        {
            tankParticles[current]->stopGenerate();
            current ++;
            current %= static_cast<char>(TankType::TT_COUNT);
            tankParticles[current]->startGenerate();
            TankPlayer::sGetInstanse()->setTankType(static_cast<TankType>(current));
        }
        else if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_LEFT) && current != 3)
        {
            tankParticles[current]->stopGenerate();
            current --;
            if (current<0)
            {
                current = static_cast<char>(TankType::TT_COUNT) - 1;
            }
            tankParticles[current]->startGenerate();
            TankPlayer::sGetInstanse()->setTankType(static_cast<TankType>(current));
        }

        if (TankinInput::sGetInstance()->isKeyDown(Action::CONFIRM) && current != 3)
        {
            Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/Level3.xml");
        }

        if (TankinInput::sGetInstance()->isKeyDown(Action::CANCEL) && current != 3)
        {
            tankParticles[current]->stopGenerate();
            current = 3; //退出选坦克界面
            currentLerpStep = 0;
            LerpStart = mCamera->getWorldPosition();
        }

        if (current == 3)
        {
            float dist = Vector3::Distance(mCamera->getWorldPosition(), mCameraPos[current]);
            if (dist <= 0.01)
            {
                mButtonNewGame->getGameObject()->activeGameObject();
                mButtonChangeLanguage->getGameObject()->activeGameObject();
                mButtonShowInput->getGameObject()->activeGameObject();
                mButtonRankList->getGameObject()->activeGameObject();
                mTitle->getGameObject()->activeGameObject();
                mCamera->setWorldPosition(mCameraPos[current]);
                mSelectingTank = false;
                current = static_cast<char>(TankPlayer::sGetInstanse()->getTankType());
            }
        }
        
        return;
    }
    if (mFirstShow)
    {
        firstShowAnimation();
        return;
    }
    
    static float currentAlpha = 1.0f;
    static float blinkSpeed = 0.02f;
    Button * button = Button::sGetCurrentSelectedButton();
    if (button != nullptr)
    {
        currentAlpha = button->getBlendFactor();
        if (currentAlpha > 0.4f)
        {
            blinkSpeed = -0.01f;
        }
        else if (currentAlpha < 0.1f)
        {
            blinkSpeed = 0.01f;
        }
        currentAlpha += blinkSpeed;
        button->setBlendFactor(currentAlpha);
    }
    
    if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_UP))
    {
        Button::sLast();
    }
    if (TankinInput::sGetInstance()->isKeyDown(Action::SELECT_DOWN))
    {
        Button::sNext();
    }
    if (TankinInput::sGetInstance()->isKeyDown(Action::CONFIRM))
    {
        Button::sTrigger();
    }
}


