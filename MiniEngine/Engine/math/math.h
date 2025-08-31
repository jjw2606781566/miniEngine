#pragma once
#include "Engine/math/MathUtils.h"
#include "Engine/pch.h"
#ifdef WIN32
#include "PC/Vector2PC.h"
#include "PC/Vector3PC.h"
#include "PC/Vector4PC.h"
#include "PC/Matrix4x4PC.h"
#include "PC/Matrix3x4PC.h"
#include "PC/QuaternionPC.h"
#include "PC/EulerAnglesPC.h"
#include "PC/RotationMatrixPC.h"
#else
#include "PS/Vector2PS.h"
#include "PS/Vector3PS.h"
#include "PS/Vector4PS.h"
#include "PS/Matrix4x4PS.h"
#include "PS/Matrix3x4PS.h"
#include "PS/QuaternionPS.h"
#include "PS/EulerAnglesPS.h"
#include "PS/RotationMatrixPS.h"
#endif

// union Float2 final
// {
//     float m[2];
//     struct { float x, y; };
//     struct { float u, v; };
//     struct { float r, g; };
//
//     Float2() : x(0.0f), y(0.0f) {}
//     Float2(float _x, float _y) : x(_x), y(_y) {}
//
//     // 拷贝构造
//     Float2(const Float2& other) : x(other.x), y(other.y) {}
//
// #ifdef ORBIS
//     operator sce::Vectormath::Scalar::Aos::Vector2() const
//     {
//         return *reinterpret_cast<const sce::Vectormath::Scalar::Aos::Vector2*>(this);
//     }
// #endif
//
//     // 拷贝赋值
//     Float2& operator=(const Float2& other)
//     {
//         x = other.x;
//         y = other.y;
//         return *this;
//     }
// };
//
//
// union Float3 final
// {
//     float m[3];
//     struct { float x, y, z; };
//     struct { float r, g, b; };
//     struct { float u, v, w; };
//
//     Float3() : x(0.0f), y(0.0f), z(0.0f) {}
//     Float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
//
//     // 拷贝构造
//     Float3(const Float3& other) : x(other.x), y(other.y), z(other.z) {}
//
// #ifdef ORBIS
//     operator sce::Vectormath::Scalar::Aos::Vector3() const
//     {
//         return *reinterpret_cast<const sce::Vectormath::Scalar::Aos::Vector3*>(this);
//     }
// #endif
//
//     // 拷贝赋值
//     Float3& operator=(const Float3& other)
//     {
//         x = other.x;
//         y = other.y;
//         z = other.z;
//         return *this;
//     }
// };
//
// union Float4 final
// {
// 	float m[4];
//     struct { float x, y, z, w; };
//     struct { float r, g, b, a; };
//     struct { float u, v; }; // 仅部分字段
//
//     Float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
//     Float4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
//
//     // 拷贝构造
//     Float4(const Float4& other) : x(other.x), y(other.y), z(other.z), w(other.w) {}
//
// #ifdef ORBIS
// 	operator sce::Vectormath::Scalar::Aos::Vector4() const
// 	{
// 		return *reinterpret_cast<const sce::Vectormath::Scalar::Aos::Vector4*>(this);
// 	}
// #endif
//
//     // 拷贝赋值
//     Float4& operator=(const Float4& other)
//     {
//         x = other.x;
//         y = other.y;
//         z = other.z;
//         w = other.w;
//         return *this;
//     }
// };
//
// union Matrix4x4 final
// {
//     Float4 rows[4];   // 行访问
//     float m[4][4];    // m[row][col] 访问
//     float data[16];   // 连续数组，便于上传到 GPU
//
//     Matrix4x4()
//     {
//         std::memset(this, 0, sizeof(Matrix4x4));
//     }
//
//     Matrix4x4(const Matrix4x4& other)
//     {
// 		std::memcpy(this, &other, sizeof(Matrix4x4));
//     }
//
// #ifdef WIN32
//     Matrix4x4(const DirectX::XMMATRIX& xm)
//     {
//         memcpy(data, &xm, sizeof(float) * 16);
//     }
//
//     operator DirectX::XMMATRIX() const
//     {
//         return *reinterpret_cast<const DirectX::XMMATRIX*>(data);
//     }
//
//     Matrix4x4 operator*(const Matrix4x4& rhs) 
//     {
//         return DirectX::XMMatrixMultiply(*this, rhs);
//     }
//
//     static Matrix4x4 Inverse(const Matrix4x4& matrix)
//     {
// 		return DirectX::XMMatrixInverse(nullptr, matrix);
//     }
// #elif defined(ORBIS)
// 	operator sce::Vectormath::Scalar::Aos::Matrix4()
// 	{
// 		return *reinterpret_cast<sce::Vectormath::Scalar::Aos::Matrix4*>(data);
// 	}
//
// 	Matrix4x4(const sce::Vectormath::Scalar::Aos::Matrix4& sce)
// 	{
//         memcpy(this, &sce, sizeof(Matrix4x4));
// 	}
//
//     Matrix4x4 operator*(const Matrix4x4& rhs) 
//     {
//         return (*reinterpret_cast<const sce::Vectormath::Scalar::Aos::Matrix4*>(&rhs)).operator*(*this);
//     }
//
//     static Matrix4x4 Inverse(const Matrix4x4& matrix)
//     {
//         return sce::Vectormath::Scalar::Aos::inverse(static_cast<const sce::Vectormath::Scalar::Aos::Matrix4>(matrix));
//     }
//
// #endif
//
//     Matrix4x4& operator=(const Matrix4x4& other)
//     {
//         if (this != &other)
//         {
//             memcpy(this, &other, sizeof(Matrix4x4));
//         }
//         return *this;
//     }
//
//     static Matrix4x4 Identity()
//     {
//         Matrix4x4 mat;
//         mat.m[0][0] = 1.0f;
//         mat.m[1][1] = 1.0f;
//         mat.m[2][2] = 1.0f;
//         mat.m[3][3] = 1.0f;
//         return mat;
//     }
//
//     Float4& operator[](size_t i) { return rows[i]; }
//     const Float4& operator[](size_t i) const { return rows[i]; }
// };