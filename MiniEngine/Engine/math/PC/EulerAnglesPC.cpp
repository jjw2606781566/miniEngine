#ifdef WIN32
#include "EulerAnglesPC.h"
#include "Engine/math/MathUtils.h"
#include "QuaternionPC.h"
#include "Matrix3x4PC.h"
#include "RotationMatrixPC.h"

// 限制集 欧拉角
void EulerAngles::canonize()
{
     // 使用 DirectXMath 提供的 wrap angle 函数
    pitch = DirectX::XMScalarModAngle(pitch);
    yaw = DirectX::XMScalarModAngle(yaw);
    roll = DirectX::XMScalarModAngle(roll);

    // 检查 pitch 是否在 [-pi/2, pi/2] 之间
    if (pitch < -DirectX::XM_PIDIV2) {
        pitch = -DirectX::XM_PI - pitch;
        yaw += DirectX::XM_PI;
        roll += DirectX::XM_PI;
    } else if (pitch > DirectX::XM_PIDIV2) {
        pitch = DirectX::XM_PI - pitch;
        yaw += DirectX::XM_PI;
        roll += DirectX::XM_PI;
    }

    // 检查万向节锁
    if (fabs(pitch) > DirectX::XM_PIDIV2 - 1e-4) {
        yaw += roll;
        roll = 0.0f;
    }
}

// 从物体-惯性坐标系四元数到欧拉角
void EulerAngles::fromObjectToInertialQuaternion(const Quaternion& quaternion) {
    float qw = quaternion.q.w;
    float qx = quaternion.q.x;
    float qy = quaternion.q.y;
    float qz = quaternion.q.z;

    // 计算 pitch
    float sp = -2.0f * (qy * qz - qw * qx);
    if (fabs(sp) >= 1.0f) {
        pitch = copysign(DirectX::XM_PIDIV2, sp);
    } else {
        pitch = asin(sp);
    }

    // 计算 yaw
    yaw = atan2(qx * qz + qw * qy, 0.5f - qx * qx - qy * qy);

    // 计算 roll
    roll = atan2(qx * qy + qw * qz, 0.5f - qx * qx - qz * qz);
}

// 从惯性-物体坐标系四元数到欧拉角
void EulerAngles::fromInertialToObjectQuaternion(const Quaternion& quaternion) {
    float qw = quaternion.q.w;
    float qx = quaternion.q.x;
    float qy = quaternion.q.y;
    float qz = quaternion.q.z;

    // 计算 pitch
    float sp = -2.0f * (qy * qz + qw * qx);
    if (fabs(sp) >= 1.0f) {
        pitch = copysign(DirectX::XM_PIDIV2, sp);
    } else {
        pitch = asin(sp);
    }

    // 计算 yaw
    yaw = atan2(-qx * qz + qw * qy, 0.5f - qx * qx - qy * qy);

    roll = atan2(-qx * qy + qw * qz, 0.5f - qx * qx - qz * qz);
}

// 从物体-世界坐标系矩阵到欧拉角
void EulerAngles::fromWorldToObjectMatrix(const Matrix3x4& matrix3_x4) {
    // 根据 m23 计算 sin(pitch)
    float sp = -matrix3_x4.m._23;

    // 检查万向锁
    if (fabs(sp) > 0.9999f) {
        pitch = MathUtils::kPiOver2 * sp;

        // roll 置零，计算 yaw
        yaw = atan2(-matrix3_x4.m._31, matrix3_x4.m._11);
        roll = 0.0f;
    } else {
        // 计算 pitch, yaw, roll
        yaw = atan2(matrix3_x4.m._13, matrix3_x4.m._33);
        pitch = asin(sp);
        roll = atan2(matrix3_x4.m._21, matrix3_x4.m._22);
    }
}

// 从世界-物体坐标系矩阵到欧拉角
void EulerAngles::fromObjectToWorldMatrix(const Matrix3x4& matrix3_x4) {
	// 根据 m23 计算 sin(pitch)
	float sp = -matrix3_x4.m._23;

	// 检查万向锁
	if (fabs(sp) > 0.9999f) {
		pitch = MathUtils::kPiOver2 * sp;

		// roll 置零，计算 yaw
		yaw = atan2(-matrix3_x4.m._31, matrix3_x4.m._11);
		roll = 0.0f;
	} else {
		// 计算 pitch, yaw, roll
		yaw = atan2(matrix3_x4.m._13, matrix3_x4.m._33);
		pitch = asin(sp);
		roll = atan2(matrix3_x4.m._21, matrix3_x4.m._22);
	}
}

void EulerAngles::fromRotationMatrix(const RotationMatrix& matrix)
{
    // 根据 23 计算 sin(pitch)
    float sp = -matrix.m._23;

    // 检查万向锁
    if (fabs(sp) > 9.99999f)
    {
        pitch = MathUtils::kPiOver2 * sp;
        yaw = atan2(-matrix.m._31, matrix.m._11);
        roll = 0.0f;
    }
    else
    {
        yaw = atan2(matrix.m._13, matrix.m._33);
        pitch = asin(sp);
        roll = atan2(matrix.m._21, matrix.m._22);
    }
}


const EulerAngles kEulerAnglesIdentity(0.0f, 0.0f, 0.0f);

#endif