#include "Engine/Utility/ThreadPool/ThreadPool.h"

#include "Engine/Application.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"

using namespace std;

#ifdef WIN32
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#if defined(DEBUG) or defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    Application::sSetDataPath("Assets/");
    
    Application::sInit();
    
    Application::sSetRunningType(EngineRunningType::Editor);
    
    Application::sRun();
    
    return 0;
}
#endif

#ifdef ORBIS
int main()
{
    Application::sSetDataPath("/app0/Assets/");

    Application::sInit();

    Application::sSetRunningType(EngineRunningType::Gameplay);

    Application::sRun();

    return 0;
}
#endif
