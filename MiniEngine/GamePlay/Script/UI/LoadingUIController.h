#pragma once
#include <future>

#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/TGUI/ImageTGUI.h"
#include "Engine/Component/TGUI/RectTransform.h"

class LoadingUIController:public MonoBehavior
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void startFileLoad();
private:
    float mCurrentProgress = 0;
    float mWidth = 1600;
    int mLoadingIsDone = 0;
    TpList<std::future<void>> tasks;
    ImageTGUI* mLoadingImage = nullptr;
    RectTransform* mLoadingImageTransform = nullptr;
    Transform* mLoadingTagTransform = nullptr;
    ImageTGUI* mLogoImage = nullptr;
    int isFirstShow = 0;
};
