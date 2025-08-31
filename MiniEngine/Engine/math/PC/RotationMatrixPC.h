#pragma once
#ifdef WIN32
#include "Engine/pch.h"

struct EulerAngles;
struct Quaternion;
struct Vector3;

struct RotationMatrix {
    DirectX::XMFLOAT3X3 m;

    RotationMatrix() : m(1.0f, 0.0f, 0.0f,
                        0.0f, 1.0f, 0.0f,
                        0.0f, 0.0f, 1.0f) {}
    
    RotationMatrix(float m11, float m12, float m13,
                   float m21, float m22, float m23,
                   float m31, float m32, float m33) : m(m11, m12, m13,
                                                         m21, m22, m23,
                                                         m31, m32, m33) {}

    // 矩阵置零
    void identity();

    // 根据欧拉角设置旋转矩阵
    void setup(const EulerAngles& orientation);

    // 根据四元数设置旋转矩阵
    void fromInertialToObjectQuaternion(const Quaternion& q);
    void fromObjectToInertialQuaternion(const Quaternion& q);

    // 执行旋转
    Vector3 inertialToObject(const Vector3& v) const;
    Vector3 objectToInertial(const Vector3& v) const;
};

extern const RotationMatrix kRoationMatrixIdentity;
#endif