#pragma once
#include "Engine/Component/ComponentHeader/TankinRenderComponent.h"
#include "Engine/math/math.h"

class Transform;
class Camera;
class CameraController final:public Component
{
public:
    void awake() override;
    void start() override;
    void update() override;
    void EditorOperation();
private:
    Transform* mTransform = nullptr;
    Camera* mCamera = nullptr;
};


