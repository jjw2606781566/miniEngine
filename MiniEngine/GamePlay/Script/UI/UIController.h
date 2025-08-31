#pragma once
#include "Engine/Component/Component.h"
#include "Engine/Component/MonoBehavior.h"
#include "Engine/Component/Audio/AudioSource.h"
#include "Engine/Component/Particle/ParticleSystem.h"
#include "Engine/Component/TGUI/Canvas.h"
#include "Engine/Component/TGUI/ImageTGUI.h"
#include "Engine/Component/TGUI/TextTGUI.h"

class Button;

class UIController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
private:
    void CreateButton();
    void firstShowAnimation();
    
    Canvas* mCanvas = nullptr;
    Transform* mTransform = nullptr;

    ImageTGUI* mTitle = nullptr;
    ImageTGUI* mInput = nullptr;

    Button* mButtonNewGame = nullptr;
    Button* mButtonChangeLanguage = nullptr;
    Button* mButtonShowInput = nullptr;
    Button* mButtonRankList = nullptr;

    Transform* mTankRoot1 = nullptr;
    Transform* mTankRoot2 = nullptr;
    Transform* mTankRoot3 = nullptr;
    TpVector<ParticleSystem*> tankParticles;

    Transform* mCamera = nullptr;

    TextTGUI* mSelectTankText = nullptr;
    TextTGUI* mFpsText = nullptr;

    AudioSource* mAudioSource = nullptr;

    //排行榜
    Transform* mRankListTransform = nullptr;
    TextTGUI* mRankText = nullptr;
    TextTGUI* mScoreText = nullptr;
    TextTGUI* mTimeText = nullptr;
    TextTGUI* mRankTextRanks = nullptr;
    TextTGUI* mScoreTextScores = nullptr;
    TextTGUI* mTimeTextTimes = nullptr;

    bool mFirstShow = true;
    bool mSelectingTank = false;
    bool mShowingInput = false;
    bool mShowingRankList = false;
    int current = 0;
    TpVector<Vector3>  mCameraPos;
};
