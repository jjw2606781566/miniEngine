#pragma once
#ifdef WIN32
#include "../../pch.h"
#include "Engine/Scene/ISerializable.h"

struct Vector2
{
    DirectX::XMFLOAT2 v;

    Vector2() : v(DirectX::XMFLOAT2(0, 0)) { }
    explicit Vector2(const DirectX::XMFLOAT2& v) : v(v) { }
    Vector2(float x, float y) : v(DirectX::XMFLOAT2(x, y)) { }

    static float Length(const Vector2& v);
    static float LengthSquared(const Vector2& v);
    static float Dot(const Vector2& v1, const Vector2& v2);
    static float Distance(const Vector2& v1, const Vector2& v2);
    static void Normalize(Vector2& v);
    static float CrossProduct(const Vector2& v1, const Vector2& v2);
    static Vector2 Abs(const Vector2& input);
    
    float Length() const;
    float LengthSquared() const;
    float Dot(const Vector2& rhs) const;
    Vector2 Normalize() const;
    float CrossProduct(const Vector2& rhs) const;
    
    float operator[](int index) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;
    Vector2 operator+(const Vector2& rhs) const;
    Vector2 operator-(const Vector2& rhs) const;
    Vector2& operator+=(const Vector2& rhs);
    Vector2& operator-=(const Vector2& rhs);
    Vector2& operator*=(float scalar);
    Vector2& operator/=(float scalar);
    bool operator==(const Vector2& rhs) const;
    bool operator!=(const Vector2& rhs) const;

    DirectX::XMVECTOR ToXMVECTOR() const;
    void FromXMVECTOR(const DirectX::XMVECTOR& vec);
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "");
    void deSerialize(const rapidxml::xml_node<>* node);
};

extern const Vector2 kZeroVector2;
#endif