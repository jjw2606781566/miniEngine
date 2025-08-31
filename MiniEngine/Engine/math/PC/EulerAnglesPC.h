#pragma once
#ifdef WIN32
#include "../../pch.h"

struct Quaternion;
struct Matrix3x4;
struct RotationMatrix;

struct EulerAngles {
    float pitch; // 绕 X 轴旋转
    float yaw;   // 绕 Y 轴旋转
    float roll;  // 绕 Z 轴旋转

    EulerAngles() : pitch(0.0f), yaw(0.0f), roll(0.0f) {}
    EulerAngles(float p, float y, float r) : pitch(p), yaw(y), roll(r) {};

    // 置零
    void identity() {pitch = yaw = roll = 0.0f;}

    // 限制集 欧拉角
    void canonize();

    // 从四元数转换到欧拉角
    void fromObjectToInertialQuaternion(const Quaternion& quaternion);
    void fromInertialToObjectQuaternion(const Quaternion& quaternion);

    // 从矩阵转换到欧拉角
    void fromObjectToWorldMatrix(const Matrix3x4& matrix3_x4);
    void fromWorldToObjectMatrix(const Matrix3x4& matrix3_x4);

    // 从旋转矩阵转换到欧拉角
    void fromRotationMatrix(const RotationMatrix& matrix);

    // static const EulerAngles kEulerAnglesIdentity;
};
extern const EulerAngles kEulerAnglesIdentity;

#endif