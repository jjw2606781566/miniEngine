#pragma once

#include "Engine/Component/ComponentHeader/TankinBaseComponent.h"
#include "Engine/Component/ComponentHeader/TankinRenderComponent.h"

class AmingLine :public MonoBehavior
{
public:
    void awake() override;
    void drawLine(const Vector3& startPos, const Vector3& startDir, float speed, float splitTime);
    void hideLine() {mAmingRoot->deactiveGameObject();}
    GameObject* getAmingTarget() const {return mCurrentAimingTarget;}
private:
    UINT32 maxCubeCount = 0;
    TpVector<Transform*> cubeTransforms;
    GameObject* mAmingRoot = nullptr;
    GameObject* mCurrentAimingTarget = nullptr;
    UINT32 currentCollisionCube = 9999999;
};