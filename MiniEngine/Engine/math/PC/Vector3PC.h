#pragma once
#ifdef WIN32
#include "Engine/pch.h"
#include "Engine/Memory/TankinMemory.h"
#include "Engine/Scene/ISerializable.h"
struct Matrix4x4;

struct Vector3
{
    DirectX::XMFLOAT3 v;

    Vector3() : v(DirectX::XMFLOAT3(0, 0, 0)) {}
    Vector3(const DirectX::XMFLOAT3& v) : v(v) {}
    Vector3(float x, float y, float z) : v(DirectX::XMFLOAT3(x, y, z)) {}

    static float Length(const Vector3& v);
    static float LengthSquared(const Vector3& v);
    static float Dot(const Vector3& v1, const Vector3& v2);
    static float Distance(const Vector3& v1, const Vector3& v2);
    static float Angle(const Vector3& v1, const Vector3& v2);
    static void Normalize(Vector3& v);
    static Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);
    static Vector3 InverseSafe(const Vector3& v);
    static Vector3 Lerp(const Vector3& start, const Vector3& end, float t);
    static Vector3 Abs(const Vector3& input);

    float Length() const;
    float LengthSquared() const;
    float Dot(const Vector3& rhs) const;
    void Scale(const Vector3& rhs);
    Vector3 Normalize() const;
    Vector3 CrossProduct(const Vector3& rhs) const;

    void TransformSelfToScreen(const Matrix4x4& mat);


    float operator[](int index) const;
    Vector3 operator*(float scalar) const;
    Vector3 operator/(float scalar) const;
    Vector3 operator+(const Vector3& rhs) const;
    Vector3 operator-(const Vector3& rhs) const;
    Vector3& operator+=(const Vector3& rhs);
    Vector3& operator-=(const Vector3& rhs);
    Vector3& operator*=(float scalar);
    Vector3& operator/=(float scalar);
    bool operator==(const Vector3& rhs) const;
    bool operator!=(const Vector3& rhs) const;

    DirectX::XMVECTOR ToXMVECTOR() const;
    void FromXMVECTOR(const DirectX::XMVECTOR& vec);

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "") ;

    void deSerialize(const rapidxml::xml_node<>* node);
};

extern const Vector3 kZeroVector3;

#endif