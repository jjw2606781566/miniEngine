#pragma once

#ifdef WIN32
#include "pch.h"
//#include "Window/WFrame.h"
#endif

class Frame;
class ThreadPool;
class IEventDispatcher;

enum EngineRunningType:uint8_t
{
    Editor,
    Gameplay,
    Debug
};

class Application
{
public:
    static const std::string& sGetDataPath() { return sDataPath; }
    static void sSetDataPath(const std::string& dataPath) { sDataPath = dataPath; }
    static ThreadPool* sGetThreadPool() {return sThreadPool;}
    static Frame* sGetFrame(){return sFrame;}
    static void sGamePlayReloadScene(const std::string& dataPath);

    //Initialize Windows or PS4, dx12, load scene, load scene resource
    static void sInit();

    //Load Configue File
    static void sLoadConfigFile();

    //MainLoop
    static void sRun();

    //running type
    static void sSetRunningType(EngineRunningType type);
    static EngineRunningType sGetRunningType() {return sRunningType;}
    
private:
    static std::string sDataPath;
    static ThreadPool* sThreadPool;
    static Frame* sFrame;
    static IEventDispatcher* sEventDispatcher;
    static bool isQuit;
    static bool isEditor;

    static EngineRunningType sRunningType;
#ifdef WIN32
#endif
    
};

