#include "EngineMode.h"

#include "Engine/Application.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/Editor/ImguiManager.h"
#include "Engine/Scene/Scene.h"
#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
#include "Engine/Dependencies/imGui/imgui_internal.h"
EngineMode* EngineMode::sInstance = nullptr;
REGISTER_EDITOR_UI(EngineMode, "EngineMode")


void EngineMode::drawSelf()
{
    ImGui::Begin("Control", nullptr,  ImGuiWindowFlags_NoResize);

    static const ImVec4  SelctedColor = ImVec4(0.5f, 0.5f, 0.5f, 1);

    if (Application::sGetRunningType() == EngineRunningType::Editor)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, SelctedColor);
        ImGui::Button("Editor");
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
    }
    else
    {
        if (ImGui::Button("Editor"))
        {
            Application::sSetRunningType(EngineRunningType::Editor);
        }
    }

    ImGui::SameLine();
    
    if (Application::sGetRunningType() == EngineRunningType::Gameplay)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, SelctedColor);
        ImGui::Button("Gameplay");
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
    }
    else
    {
        if (ImGui::Button("Gameplay"))
        {
            Application::sSetRunningType(EngineRunningType::Gameplay);
        }
    }

    ImGui::SameLine();

    if (Application::sGetRunningType() == EngineRunningType::Debug)
    {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleColor(ImGuiCol_Button, SelctedColor);
        ImGui::Button("Debug");
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();
    }
    else
    {
        if (ImGui::Button("Debug"))
        {
            Application::sSetRunningType(EngineRunningType::Debug);
        }
    }

    if (ImGui::Button("BoundingBox"))
    {
        Camera::sGetMainCamera()->setRenderCollider(!Camera::sGetMainCamera()->getRenderCollider());
    }

    if (Application::sGetRunningType() == EngineRunningType::Editor)
    {
        ImGui::SameLine();
    
        if (ImGui::Button("SaveScene"))
        {
            Scene::sSaveScene();
            ImGui::OpenPopup("Save Success");
        }

        ImGui::SameLine();
        static bool isMain = Camera::sGetMainCamera()->getGameObject()->isActive();
        if (ImGui::Button("UI"))
        {
            if (isMain)
            {
                Camera::sGetMainCamera()->getGameObject()->deactiveGameObject();
                Transform* uicamera = Transform::sFindGameObject("UIMainCamera");
                ASSERT(uicamera != nullptr, TEXT("UIMainCamera is nullptr"));
                uicamera->getGameObject()->activeGameObject();
                isMain = false;
            }
            else
            {
                Camera::sGetMainCamera()->getGameObject()->activeGameObject();
                Transform* uicamera = Transform::sFindGameObject("UIMainCamera");
                ASSERT(uicamera != nullptr, TEXT("UIMainCamera is nullptr"));
                uicamera->getGameObject()->deactiveGameObject();
                isMain = true;
            }
        }
    }

    if (ImGui::BeginPopupModal("Save Success", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove)) {
        ImGui::Text("Save operation was successful!");
        ImGui::Separator();

        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup(); // 关闭模态窗
        }

        ImGui::EndPopup();
    }
    
    ImGui::End();
}

EngineMode* EngineMode::sGetInstance()
{
    if (sInstance == nullptr)
    {
        sInstance = new EngineMode();
    }
    return sInstance;
}
#endif
