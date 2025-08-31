#pragma once
#include "Engine/math/math.h"

struct LightConstant
{
    Vector4 mMainLightDir;   // (Direction Light)1 : direction / (Point Light) 2 : position
    Vector3 mMainLightColor; // light color (float3) + intensity (float)
    float mMainLightIntensity;
    Vector4 mShadowColor;
    Vector3 mAmbientLight;   // ambient light (float3) + ambient intensity
    float mAmbientIntensity;
};

struct FogConstant
{
    Vector4 mFogColor;   // fogColor.xyz | fog density
    Vector4 mFogParams;  // fogStart | fogEnd | preserved | preserved
};

struct CameraConstants
{
    Matrix4x4 mView;
    Matrix4x4 mViewInverse;
    Matrix4x4 mProjection;
    Matrix4x4 mProjectionInverse;
};

struct InstanceData
{
    Matrix4x4 mModel;
    Matrix4x4 mModelInverse;
};

struct alignas(256) TransformConstants
{
    Matrix4x4 mModel;
    Matrix4x4 mModelInverse;
    CameraConstants mCameraTransforms;
};