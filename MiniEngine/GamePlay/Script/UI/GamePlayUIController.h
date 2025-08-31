#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/TGUI/Button.h"
#include "Engine/Component/TGUI/TextTGUI.h"

class GamePlayUIController : public MonoBehavior
{
public:
    void awake() override;
    void update() override;
private:
    GameObject* mStopRoot = nullptr;
    TextTGUI* mContinueText = nullptr;
    Button* mContinueButton = nullptr;
    TextTGUI* mBackToMainText = nullptr;
    Button* mBackToMainButton = nullptr;
    TextTGUI* mScoreText = nullptr;
    TextTGUI* mTimeText = nullptr;

    TextTGUI* mFpsText = nullptr;
};
