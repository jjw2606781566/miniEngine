#pragma once
#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"

class DelayDestruction:public MonoBehavior
{
public:
    void awake() override;
    void update() override;
    void startTiming(){mIsRunning = true;}
    void setDelayTime(float delayTime){mDelayTime = delayTime;}
    
private:
    float mDelayTime = 1.0f;
    float mCurrentTime = 0.0f;
    bool mIsRunning = false;
};
