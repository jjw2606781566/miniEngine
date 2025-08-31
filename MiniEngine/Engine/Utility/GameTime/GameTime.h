#pragma once

#include <atomic>
#include <chrono>
#include <string>

#include "../MacroUtility.h"
#include "Engine/Application.h"


class GameTime
{
public:
    static void sInit();

    static void sUpdate();
    static void sFreshGameTime();

    //Physics update, develop later
    static uint64_t sGetFixedFrameCount() {return sFixedFrameCount;}
    static uint64_t sGetFixedUpdateCount()
    {
        uint64_t result = sFixedFrameCount - sCurrentFixedFrameCount;
        sCurrentFixedFrameCount = sFixedFrameCount;
        return result;
    }

    static double sGetFixedDeltaTime();

    static double sGetDeltaTime();

    static void sPause(){sIsPaused = true;}

    static void sResume(){sIsPaused = false;}

    static bool sGetIsPaused() {return sIsPaused;}

    static double getCurrentFps()
    {
        //std::string text = std::string("DeltaTime: ")+ std::to_string(sDeltaTime)+ std::string("\n");
        //OutputDebugStringA(text.c_str());
        return 1.0 / sDeltaTime;
    }

    //limit FPS
    static void sSetLimitedFps(uint32_t fps);
    static uint32_t sGetLimitedFps();

    static void sSetTimeScale(float scale) {sTimeScale = scale;};
    
private:
    GameTime(){}

    //last update time
    static std::chrono::high_resolution_clock::time_point lastUpdate;
    
    //SecondPerFrame, do not influence by any other factors
    
    static double sDeltaTime;
    static double sDeltaTimeBuffer;
    
    //Physics update deltaTime, is a fixed value
    static const float sFixedDeltaTime;
    static double sAccumulatedFixedDeltaTime;
    static uint64_t sFixedFrameCount;
    static uint64_t sCurrentFixedFrameCount;
    
    static bool sIsPaused;

    static float sTimeScale;

    //limited FPS
    static double sLimitedMilliSecondPerFrame;
    static std::chrono::high_resolution_clock::time_point sNextTimePoint;
};

