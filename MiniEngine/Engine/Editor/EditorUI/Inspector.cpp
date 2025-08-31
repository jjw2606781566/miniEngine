#include "Inspector.h"

#include "Engine/Editor/ImguiManager.h"

#ifdef WIN32
Inspector* Inspector::sInstance = nullptr;
REGISTER_EDITOR_UI(Inspector, "Inspector")

void Inspector::drawSelf()
{
    ImGui::Begin("Inspector");
    if (mSelectedGameObject != nullptr)
    {
        mSelectedGameObject->showSelf();
    }
    ImGui::End();
}

Inspector* Inspector::sGetInstance()
{
    if (sInstance == nullptr)
    {
        sInstance = new Inspector();
        //向destroy方法注册，防止访问无效指针
        GameObjectFactory::sRegisterDestroyCallBack([](GameObject* thisGo)
        {
           if (thisGo == sInstance->mSelectedGameObject)
           {
               sInstance->mSelectedGameObject = nullptr;
           } 
        });
    }
    return sInstance;
}


#endif

