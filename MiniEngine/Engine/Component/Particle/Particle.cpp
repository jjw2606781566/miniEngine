#include "Particle.h"

Matrix4x4 Particle::getModelMatrix() const
{
    Matrix4x4 model;
    Quaternion q;
    model.setModelMatrixQuaternion(mPosition, q, {0.01f * mSizeScale,0.01f * mSizeScale,0.01f * mSizeScale});
    return model;
}

void Particle::update(float deltatime, float airResistanceCoefficient )
{
    Vector3 finalVelocity = mVelocity + mAcceleration * deltatime - 
                             mVelocity * airResistanceCoefficient;
    mPosition += (mVelocity * deltatime +  mAcceleration * deltatime * deltatime * 0.5f);
    mVelocity = finalVelocity;

    mRemainLifeTime -= deltatime;
    if (mRemainLifeTime <= 0)
    {
        mIsDie = true;
    }
}
