#pragma once
#include "Engine/math/math.h"
#include "Engine/render/Color.h"
#include "Engine/render/RenderItem.h"

#define PARTICLE_GRAVITY 9.8f

struct Particle
{
    Vector3 mPosition;
    Vector3 mVelocity;
    Vector3 mAcceleration;
    float mSizeScale = 1.0f;
    bool mIsDie = true;
    float mRemainLifeTime = 1.0f;

    std::unique_ptr<MaterialInstance> mMaterialGpu = nullptr;

    Matrix4x4 getModelMatrix() const;
    void update(float deltatime, float airResistanceCoefficient );
};
