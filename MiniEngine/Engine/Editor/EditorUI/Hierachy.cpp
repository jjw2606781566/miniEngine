#include "Hierachy.h"

#include "Inspector.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/TGUI/Canvas.h"
#include "Engine/Component/RenderComponent/Camera.h"
#include "Engine/Component/RenderComponent/MeshFilter.h"
#include "Engine/Component/RenderComponent/MeshRenderer.h"
#include "Engine/Component/TGUI/RectTransform.h"
#include "Engine/Editor/ImguiManager.h"

#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"

REGISTER_EDITOR_UI(Hierachy, "Hierachy")

Hierachy* Hierachy::sInstance = nullptr;

Hierachy* Hierachy::sGetInstance()
{
    if (sInstance == nullptr)
    {
        sInstance = new Hierachy();
    }
    return sInstance;
}

void Hierachy::drawSelf()
{
    const float fixedHeight = 900;
    const float minWidth = 200;
    const float maxWidth = 900;
    ImGui::SetNextWindowSizeConstraints(ImVec2(minWidth, fixedHeight)
        , ImVec2(maxWidth, fixedHeight));
    Transform* root = Transform::sGetRoot();
    if (ImGui::Begin("Hierarchy", nullptr))
    {
        openRightClickMenu(root);
    }
    ImGuiTreeNodeFlags baseFlags =
        ImGuiTreeNodeFlags_OpenOnDoubleClick
        | ImGuiTreeNodeFlags_OpenOnArrow
        | ImGuiTreeNodeFlags_SpanAvailWidth;
    
    auto children = root->getChildren();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    for (auto& child : children)
    {
        drawHierarchy(child, baseFlags);
    }
    ImGui::PopStyleColor();
    ImGui::End();
}

void Hierachy::drawHierarchy(Transform* transform, ImGuiTreeNodeFlags baseFlags)
{
    ImGuiTreeNodeFlags thisFlags = baseFlags;
    if (mSelecedTransform == transform)
    {
        thisFlags |= ImGuiTreeNodeFlags_Selected;
    }
    if (!transform->getGameObject()->isActive())
    {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }
    auto children = transform->getChildren();
    if (children.size() > 0)
    {
        bool result = ImGui::TreeNodeEx(transform->getGameObjectName().c_str(), thisFlags);
        openRightClickMenu(transform);
        if (ImGui::IsItemClicked())
        {
            mSelecedTransform = transform;
            Inspector::sGetInstance()->setGameObject(mSelecedTransform->getGameObject());
        }
        if (result)
        {
            for (auto& child:children)
            {
                drawHierarchy(child, baseFlags);
            }
            //只有可以展开的node才需要pop
            ImGui::TreePop();
        }
    }
    else
    {
        thisFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(transform->getGameObjectName().c_str(), thisFlags);
        openRightClickMenu(transform);
        if (ImGui::IsItemClicked())
        {
            mSelecedTransform = transform;
            Inspector::sGetInstance()->setGameObject(mSelecedTransform->getGameObject());
        }
    }

    if (!transform->getGameObject()->isActive())
    {
        ImGui::PopStyleColor();
    }
}

void Hierachy::openRightClickMenu(Transform* transform)
{
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("CameraMoveToThis")) {
            Camera* mainCamera = Camera::sGetMainCamera();
            mainCamera->getGameObject()->getTransform()->setWorldPosition(transform->getWorldPosition());
        }

        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("CreateEmpty"))
            {
                GameObject* go = GameObjectFactory::sCreateGameObject("Empty");
                go->addTransform(transform);
            }
            if (ImGui::BeginMenu("Create3D"))
            {
                if (ImGui::MenuItem("CreateDefaultCube"))
                {
                    GameObject* go = GameObjectFactory::sCreateGameObject("DefaultCube");
                    go->addTransform(transform);
                    go->addComponent("MeshFilter");
                    go->addComponent("MeshRenderer");
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("CreateUI"))
            {
                if (ImGui::MenuItem("CreateImage"))
                {
                    Canvas* canvas = getGameObjectCanvas(transform);
                    if (canvas != nullptr)
                    {
                        GameObject* go = GameObjectFactory::sCreateGameObject("Image");
                        go->addRectTransform(transform, canvas);
                        go->addComponent("ImageTGUI");
                    }
                }
                if (ImGui::MenuItem("CreateButton"))
                {
                    Canvas* canvas = getGameObjectCanvas(transform);
                    if (canvas != nullptr)
                    {
                        GameObject* go = GameObjectFactory::sCreateGameObject("NewButton");
                        go->addRectTransform(transform, canvas);
                        go->addComponent("Button");
                    }
                }
                if (ImGui::MenuItem("CreateText"))
                {
                    Canvas* canvas = getGameObjectCanvas(transform);
                    if (canvas != nullptr)
                    {
                        GameObject* go = GameObjectFactory::sCreateGameObject("NewText");
                        go->addRectTransform(transform, canvas);
                        go->addComponent("TextTGUI");
                    }
                }
                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }

        //不允许对root进行的操作
        if (transform != Transform::sGetRoot())
        {
            if (ImGui::MenuItem("UpSelfToFirstChild"))
            {
                transform->upSelfToFirstChild();
            }
            
            if (ImGui::MenuItem("DownToLastChild"))
            {
                transform->downSelfToLastChild();
            }
            
            if (ImGui::MenuItem("Delete")) {
                GameObject* go = transform->getGameObject();
                GameObjectFactory::sDestroyGameObject(go);
            }
        }
        
        ImGui::EndPopup();
    }
}

Canvas* Hierachy::getGameObjectCanvas(Transform* transform)
{
    Canvas* canvas = dynamic_cast<Canvas*>(transform->getGameObject()->getComponent("Canvas"));
    if (canvas)
        return canvas;
    RectTransform* rectTransform = dynamic_cast<RectTransform*>(transform);
    if (rectTransform == nullptr)
        return nullptr;
    canvas = rectTransform->getCanvas();
    ASSERT(canvas, TEXT("Canvas is nullptr!"));
    return canvas;
}
#endif
