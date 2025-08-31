#pragma once
#include "Engine/Memory/TankinMemory.h"
#include "Engine/Scene/ISerializable.h"

#ifdef WIN32
#include "../../pch.h"

struct Vector3;
struct Quaternion;
struct EulerAngles;
struct Matrix4x4;


struct Quaternion
{
    DirectX::XMFLOAT4 q;

    //Empty Quaternion
    Quaternion() : q(0.0f, 0.0f, 0.0f, 1.0f) {}
    Quaternion(float x, float y, float z, float w) : q(x, y, z, w) {}

    // 执行旋转的四元数
    void setToRotateAboutX(float theta);
    void setToTotateAboutY(float theta);
    void setToTotateAboutZ(float theta);
    void setToRotateAboutAxis(const Vector3& axis, float theta);

    // 执行物体-惯性坐标系旋转的四元数
    void setQuaternionRotationRollPitchYaw(const Vector3& eularAngle);

    // 叉乘
    Quaternion operator *(const Quaternion& a) const;

    Quaternion operator *=(const Quaternion& a);

    // rotate a direction
    void QuaternionRotateVector(Vector3& vec) const;

    Quaternion getInverse() const;

    // 正则化四元数
    void normalize();

    // 提取旋转角度和轴
    float getRotationAngle() const;
    Vector3 getRotationAxis() const;

    //transform to euler angles
    Vector3 getEulerAnglesDegree() const;
    
    //serialization
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "");
    void deSerialize(const rapidxml::xml_node<>* node);
};

// empty Quaternion
extern const Quaternion kQuaternionEmpty;

// 四元数的点乘
extern float dotProduct(const Quaternion& a, const Quaternion& b);

// 四元数的球面线性插值
extern Quaternion slerp(const Quaternion& p, const Quaternion& q, float t);

// 四元数的共轭
extern Quaternion conjugater(const Quaternion& q);

// 四元数的幂
extern Quaternion pow(const Quaternion& q, float exponent);

#endif