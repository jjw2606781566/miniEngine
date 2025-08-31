#pragma once

#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/TGUI/TextTGUI.h"

class EndUIController:public MonoBehavior
{
public:
    void awake() override;
    void update() override;
private:
    void firstUpdateAnimation();
    TextTGUI* mContinueTextEng = nullptr;
    ImageTGUI* mCai = nullptr;

    bool mIsFirstShow = true;
};
