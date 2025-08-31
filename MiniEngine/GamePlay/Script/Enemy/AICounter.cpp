#include "AICounter.h"

#include "Engine/Application.h"

AICounter* AICounter::sInstance = nullptr;

AICounter* AICounter::getInstance()
{
    if (!sInstance)
    {
        sInstance = new AICounter();
    }
    return sInstance;
}

void AICounter::increaceValue()
{
    value++;
}

void AICounter::decreaseValue()
{
    value--;
}

void AICounter::setZero()
{
    value = 0;
}

void AICounter::nextLevel()
{
    if (mLevel == Level::LEVEL_COMMON)
    {
        Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/Level4.xml");
        mLevel = Level::LEVEL_BOSS;
    }
    else if (mLevel == Level::LEVEL_BOSS)
    {
        Application::sGamePlayReloadScene(Application::sGetDataPath() + "Scene/WinScene.xml");
        mLevel = Level::LEVEL_COMMON;
    }
}

