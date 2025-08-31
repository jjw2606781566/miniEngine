#pragma once
#ifdef WIN32
#include "../../pch.h"


struct Vector3;
struct EulerAngles;
struct Quaternion;
struct RotationMatrix;

struct Matrix4x4 {
    DirectX::XMFLOAT4X4 m;

    Matrix4x4() : m(
       1.0f, 0.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f, 0.0f,
       0.0f, 0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 0.0f, 1.0f
   ) {}

    Matrix4x4(float m11, float m12, float m13, float m14,
              float m21, float m22, float m23, float m24,
              float m31, float m32, float m33, float m34,
              float m41, float m42, float m43, float m44) 
        : m(m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34,
            m41, m42, m43, m44) {}

    void setTranslation(float tx, float ty, float tz);
    void setRotationX(float theta);
    void setRotationY(float theta);
    void setRotationZ(float theta);
    
    void setScale(float sx, float sy, float sz);
    void setPerspectiveProjection(float fov, float aspect, float zn, float zf);
    void setOrthographicProjection(float width, float height, float zn, float zf);
    void setView(const Vector3& positon, const Vector3& forward, const Vector3& up, const Vector3& right);
    void lookAt(const Vector3& positon, const Vector3& target, const Vector3& up);
    void setModelMatrix(const Vector3&position, const Vector3& rotation, const Vector3& scale);
    void setModelMatrixQuaternion(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

    void inverseSelf();
    Matrix4x4 transpose() const;

    Matrix4x4 operator*(const Matrix4x4& other)const;

    static const Matrix4x4 Identity;
    static const Matrix4x4 Zero;
};

#endif