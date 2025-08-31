#include <iostream>

#include "Editor/ImguiManager.h"
#include "Utility/EventDispatcher/EventDispatcher.h"
#include "Application.h"

#include "AudioSystem/AudioInterface.h"
#include "Component/GameObject.h"
#include "Component/Transform.h"
#include "Component/RenderComponent/Camera.h"

#include "Dependencies/rapidxml/rapidxml_utils.hpp"
#include "Utility/GameTime/GameTime.h"
#include "Utility/ThreadPool/ThreadPool.h"
#include "Memory/TankinMemory.h"
#include "render/Renderer.h"
#include "Scene/Scene.h"
#include "InputControl/TankinInput.h"
#include "Physical/PhysicSystem.h"

#include "Window/Frame.h"
#include "Window/WFrame.h"

#ifdef WIN32
#include "Dependencies/imGui/imgui_impl_win32.h"
#endif

ThreadPool* Application::sThreadPool = nullptr;
std::string Application::sDataPath;
Frame* Application::sFrame = nullptr;
IEventDispatcher* Application::sEventDispatcher = nullptr;
bool Application::isEditor = true;
bool Application::isQuit = false;
EngineRunningType Application::sRunningType = EngineRunningType::Editor;

void Application::sGamePlayReloadScene(const std::string& dataPath)
{
   sRunningType = Editor;
   Scene::sSceneFilePath = dataPath;
   Scene::reLoadScene();
   
   sRunningType = Gameplay;
   Scene::sDirectEnterGamePlay = true;
}

void Application::sInit()
{
   //Load configure file
   Application::sLoadConfigFile();

   //need Initiate self first
   sThreadPool = new ThreadPool(8, 32, 300, RejectionPolicy::ThrowException);
   
   //Some Init, Must follow some order!!!
   TankinInput::sInit();
   PhysicSystem::getInstance();
   isQuit = false;
   AudioInterface::sInit();
   
   //windows, need Init after Input and ImGui
#ifdef WIN32
   //1710, 972
   sFrame = WFrame::CreateFrame(TEXT("Sample Frame"), 1920, 1080, true, InputProc);
   sEventDispatcher = IEventDispatcher::CreateDispatcher();
   RAWINPUTDEVICE rid[1];
   rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;  // HID_USAGE_PAGE_GENERIC
   rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;      // HID_USAGE_GENERIC_MOUSE
   rid[0].dwFlags = RIDEV_INPUTSINK;
   rid[0].hwndTarget = GetActiveWindow();
   
   ImGui_ImplWin32_Init(GetActiveWindow());
   
   ASSERT(RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE)) == TRUE,
       TEXT("RegisterRawInputDevices FALSE!"))
#endif
#ifdef ORBIS
       sFrame = new Frame(L"frame",1920, 1080, true);
#endif
   //Renderer
   Renderer::GetInstance().initialize();
#ifdef WIN32
   ImguiManager::sGetInstance()->init();
#endif

   //GameTime
   GameTime::sInit();
   
}

void Application::sLoadConfigFile()
{
   TpString path = sDataPath + "Configuration/EngineConfigue.xml";
   rapidxml::file<>* xmlFile = new rapidxml::file<>(path.c_str());
   rapidxml::xml_document<>* doc = new rapidxml::xml_document<>();

   doc->parse<0>(xmlFile->data());
   auto rootnode = doc->first_node("Engine");
   Scene::sSceneFilePath = sDataPath + rootnode->first_attribute("DefaultScene")->value();
}

void Application::sRun()
{
   GameTime::sSetLimitedFps(60);
   std::vector<std::future<void>> tasks;
   /*tasks.push_back(sThreadPool->enqueue(1,[]()
   {
      while (true)
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(5000));
         std::cout<<"Thread FPS: "<<GameTime::getCurrentFps()<<std::endl;
      }
   }));*/ 
   
   //MainLoop
#ifdef WIN32
   while (sEventDispatcher->Dispatch() && !isQuit)
#endif
#ifdef ORBIS
   	while (!isQuit)
#endif
   {
      if (Scene::sNeedLoadScene)
      {
         Scene::sLoadScene();
         GameTime::sFreshGameTime(); //avoid loading time influence fps compute
         if (Scene::sDirectEnterGamePlay)
         {
            Component::sAwakeAllMonoBehavior();
            Scene::sDirectEnterGamePlay = false;
         }
      }
      
      DEBUG_PRINT("---------------------New Frame------------------------\n");

      Component::sFixedUpdateAllComponent();

      //Physics
      //do not update in Editor mode
      if (sRunningType != EngineRunningType::Editor)
      {
         PhysicSystem::getInstance().update(GameTime::sGetFixedDeltaTime());
      }
      
      //GameLogic
      Component::sUpdateAllComponent();
      
      //imGui
#ifdef WIN32
      ImguiManager::sGetInstance()->flushFrame();
#endif
      
      //render
      DEBUG_PRINT("Render Starts\n");
      Camera::sRenderScene(); 
      
      DEBUG_PRINT("Render End\n");
      

      //Update Timer
      GameTime::sUpdate();

      TankinInput::sGetInstance()->update();

      ComponentFactory::sGarbageCollect();
      GameObjectFactory::sGarbageCollect();
   }
}

void Application::sSetRunningType(EngineRunningType type)
{
   auto lastType = sRunningType;
   sRunningType = type;
   switch (sRunningType)
   {
      case EngineRunningType::Gameplay:
         if (lastType == EngineRunningType::Editor)
            Component::sAwakeAllMonoBehavior();
         break;
      case EngineRunningType::Editor:
         Scene::reLoadScene();
         break;
      case EngineRunningType::Debug:
         if (lastType == EngineRunningType::Editor)
            Component::sAwakeAllMonoBehavior();
         break;
   }
   
}

