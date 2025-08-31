#include "DelayDestruction.h"

#include "Engine/Utility/GameTime/GameTime.h"

REGISTER_COMPONENT(DelayDestruction, "DelayDestruction")

void DelayDestruction::awake()
{
    
}

void DelayDestruction::update()
{
    if (mIsRunning)
    {
        mCurrentTime += GameTime::sGetDeltaTime();
        if (mCurrentTime >= mDelayTime)
        {
            GameObjectFactory::sDestroyGameObject(mGameObject);
        }
    }
}
