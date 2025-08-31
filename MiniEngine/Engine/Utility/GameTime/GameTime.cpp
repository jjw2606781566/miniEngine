#include "GameTime.h"

#include <future>

#include "Engine/Application.h"


std::chrono::high_resolution_clock::time_point GameTime::lastUpdate = std::chrono::high_resolution_clock::now();
double GameTime::sDeltaTime = 0.02f;
double GameTime::sDeltaTimeBuffer;
const float GameTime::sFixedDeltaTime = 0.02f;
bool GameTime::sIsPaused = false;
double GameTime::sLimitedMilliSecondPerFrame = 0.0f;
uint64_t GameTime::sFixedFrameCount = 0;
uint64_t GameTime::sCurrentFixedFrameCount = 0;
double GameTime::sAccumulatedFixedDeltaTime = 0;
std::chrono::high_resolution_clock::time_point GameTime::sNextTimePoint = std::chrono::high_resolution_clock::now();
float GameTime::sTimeScale = 1;

void GameTime::sInit()
{
    //load time configure from file

    lastUpdate = std::chrono::high_resolution_clock::now();
    sFixedFrameCount = 0;
    //sDeltaTime = 0.0f;
}

double GameTime::sGetDeltaTime()
{
    //pause do not influence sDeltatime computation in every frame
    if (sIsPaused)
        return 0.0f;
    else
    {
        return sDeltaTime * sTimeScale;
    }
}

double GameTime::sGetFixedDeltaTime()
{
    if (sIsPaused)
        return 0.0f;
    else
    {
        return sFixedDeltaTime * sTimeScale;
    }
}

void GameTime::sUpdate()
{
    std::chrono::high_resolution_clock::time_point current = std::chrono::high_resolution_clock::now();
    uint64_t microSecond = std::chrono::duration_cast<std::chrono::microseconds>(current-lastUpdate).count();
    sDeltaTimeBuffer = static_cast<double>(microSecond) / 1000000.0f;
    //std::string text = std::string("DeltaTime: ")+ std::to_string(sDeltaTime)+ std::string("\n");
    //OutputDebugStringA(text.c_str());

    if (sLimitedMilliSecondPerFrame > 0.0f)
    {
        sNextTimePoint = current + std::chrono::nanoseconds(static_cast<int>(sLimitedMilliSecondPerFrame * 1000000.0f));
        
        current = std::chrono::high_resolution_clock::now();
        //allow some error in 1.5millisecond
        auto allowErrorMillisecond = std::chrono::microseconds(1500);
        
        while ((current + allowErrorMillisecond) < sNextTimePoint)
        {
            current = std::chrono::high_resolution_clock::now();
        }
        microSecond = std::chrono::duration_cast<std::chrono::microseconds>(current - lastUpdate).count();
        sDeltaTimeBuffer = static_cast<double>(microSecond) / 1000000.0f;
    }

    //Update FixedFrame
    if (!sIsPaused) //when pause, physics need pause too
    {
        sAccumulatedFixedDeltaTime += sDeltaTimeBuffer;
        auto tempAccumu = sAccumulatedFixedDeltaTime;
        auto temp = sDeltaTimeBuffer;
        while (sAccumulatedFixedDeltaTime > sFixedDeltaTime)
        {
            sFixedFrameCount++;
            sAccumulatedFixedDeltaTime -= sFixedDeltaTime;
        }
    }
    
    current = std::chrono::high_resolution_clock::now();
    microSecond = std::chrono::duration_cast<std::chrono::microseconds>(current - lastUpdate).count();
    sDeltaTime = static_cast<double>(microSecond) / 1000000.0f;
    lastUpdate = sNextTimePoint;
}

void GameTime::sFreshGameTime()
{
    lastUpdate = std::chrono::high_resolution_clock::now();
}

void GameTime::sSetLimitedFps(uint32_t fps)
{
    //convert fps to Millisecond per frame
    sLimitedMilliSecondPerFrame = 1000.0f / static_cast<double>(fps);
    //DEBUG_PRINT(std::string("sLimitedMilliSecondPerFrame: ") + STRING(sLimitedMilliSecondPerFrame) + std::string("\n"));
}

uint32_t GameTime::sGetLimitedFps()
{
    //convert Millisecond per frame to fps
    return static_cast<uint32_t>(1000.0f / sLimitedMilliSecondPerFrame);
}