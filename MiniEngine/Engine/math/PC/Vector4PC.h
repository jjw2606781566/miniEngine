#pragma once
#ifdef WIN32
#include "Engine/pch.h"
#include "Engine/Dependencies/rapidxml/rapidxml.hpp"
#include "Engine/Memory/TankinMemory.h"
struct Matrix4x4;

struct Vector4
{
    DirectX::XMFLOAT4 v;

    Vector4() : v(DirectX::XMFLOAT4(0, 0, 0, 0)) {}
    Vector4(const DirectX::XMFLOAT4& v) : v(v) {}
    Vector4(float x, float y, float z, float w) : v(DirectX::XMFLOAT4(x, y, z, w)) {}

    static float Length(const Vector4& v);
    static float LengthSquared(const Vector4& v);
    static float Dot(const Vector4& v1, const Vector4& v2);
    static float Distance(const Vector4& v1, const Vector4& v2);
    static float Angle(const Vector4& v1, const Vector4& v2);
    static void Normalize(Vector4& v);
    static Vector4 CrossProduct(const Vector4& v1, const Vector4& v2, const Vector4& v3);
    static Vector4 InverseSafe(const Vector4& v);
    static Vector4 Lerp(const Vector4& start, const Vector4& end, float t);
    static Vector4 Abs(const Vector4& input);

    float Length() const;
    float LengthSquared() const;
    float Dot(const Vector4& rhs) const;
    void Scale(const Vector4& rhs);
    Vector4 Normalize() const;
    Vector4 CrossProduct(const Vector4& rhs, const Vector4 rlhs) const;

    void TransformSelfToScreen(const Matrix4x4& mat);


    float operator[](int index) const;
    Vector4 operator*(float scalar) const;
    Vector4 operator/(float scalar) const;
    Vector4 operator+(const Vector4& rhs) const;
    Vector4 operator-(const Vector4& rhs) const;
    Vector4& operator+=(const Vector4& rhs);
    Vector4& operator-=(const Vector4& rhs);
    Vector4& operator*=(float scalar);
    Vector4& operator/=(float scalar);
    bool operator==(const Vector4& rhs) const;
    bool operator!=(const Vector4& rhs) const;

    DirectX::XMVECTOR ToXMVECTOR() const;
    void FromXMVECTOR(const DirectX::XMVECTOR& vec);

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "")
    ;
    void deSerialize(const rapidxml::xml_node<>* node);
};
#endif