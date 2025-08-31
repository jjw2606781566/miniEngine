#pragma once
#ifdef WIN32
#include "../../pch.h"

struct Vector3;
struct EulerAngles;
struct Quaternion;
struct RotationMatrix;

struct Matrix3x4 {
    DirectX::XMFLOAT3X4 m;

    Matrix3x4() : m(1.0f, 0.0f, 0.0f, 0.0f,
                   0.0f, 1.0f, 0.0f, 0.0f,
                   0.0f, 0.0f, 1.0f, 0.0f) {}
    
    Matrix3x4(float m11, float m12, float m13, float m14,
              float m21, float m22, float m23, float m24,
              float m31, float m32, float m33, float m34) 
        : m(m11, m12, m13, m14,
            m21, m22, m23, m24,
            m31, m32, m33, m34) {}

    // 矩阵置零
    void identity();

    // 平移
    void zeroTranslation();
    void setTranslation(const Vector3& d);
    void setupTranslation(const Vector3& d);

    // 父空间<->局部空间
    void setupLocalToParent(const Vector3& pos, const EulerAngles& orient);
    void setupLocalToParent(const Vector3& pos, const RotationMatrix& orient);
    void setupParentToLocal(const Vector3& pos, const EulerAngles& orient);
    void setupParentToLocal(const Vector3& pos, const RotationMatrix& orient);

    // 构造绕坐标轴旋转的矩阵
    void setupRotate(int axis, float theta);

    //构造yaw、pitch、roll旋转矩阵
    void setupRotation(const Vector3& rotation);

    // 构造绕任意轴旋转的矩阵
    void setupRotate(const Vector3& axis, float theta);

    // 构造旋转矩阵，四元数作角位移
    void fromQuaternion(const Quaternion& q);

    // 构造沿坐标轴缩放的矩阵
    void setupScale(const Vector3& s);

    // 构造沿任意轴缩放的矩阵
    void setupScaleAlongAxis(const Vector3& axis, float k);
    // 构造切变矩阵
    void setupShear(int axis, float s, float t);

    // 构造投影矩阵，投影平面过原点
    void setupProject(const Vector3& n);
    
    // 构造反射矩阵
    void setupReflect(int axis, float k = 0.0f);

    // 构造沿任意平面反射的矩阵
    void setupReflect(const Vector3& n);

    DirectX::XMMATRIX ToXMMATRIX() const;
    static Matrix3x4 FromXMMATRIX(const DirectX::XMMATRIX& mat);

   
};

// 用运算符 * 变换点或连接矩阵，乘法顺序从左向右进行
Vector3 operator*(const Matrix3x4& m, const Vector3& v);
Matrix3x4 operator*(const Matrix3x4& m, const Matrix3x4& n);


// 计算3x3部分的行列式
float determinant(const Matrix3x4& m);

// 计算矩阵的逆
Matrix3x4 inverse(const Matrix3x4& m);

// 提取矩阵的平移部分
Vector3 getTranslation(const Matrix3x4& m);

// 从局部矩阵 -> 父矩阵 -> 局部矩阵方位
Vector3 getPositionFromParentToLocalMatrix(const Matrix3x4& m);
Vector3 getPositionFromLocalToParentMatrix(const Matrix3x4& m);

extern const Matrix3x4 kMatrixIdentity; 
#endif