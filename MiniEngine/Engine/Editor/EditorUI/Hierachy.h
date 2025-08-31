#pragma once
#include "Engine/Component/Transform.h"
#include "Engine/Editor/EditorUi.h"

#ifdef WIN32
#include "Engine/Dependencies/imGui/imgui.h"
class Canvas;

class Hierachy : public EditorUi
{
public:
    static Hierachy* sGetInstance();
    void drawSelf() override;
    
private:
    Hierachy() = default;
    DELETE_CONSTRUCTOR_FIVE(Hierachy)
    static Hierachy* sInstance;

    void drawHierarchy(Transform* transform, ImGuiTreeNodeFlags baseFlags);
    void openRightClickMenu(Transform* transform);
    Canvas* Hierachy::getGameObjectCanvas(Transform* transform);
    
    Transform* mSelecedTransform = nullptr;
};
#endif
