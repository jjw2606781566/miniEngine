#ifdef WIN32
#include "RotationMatrixPC.h"
#include "EulerAnglesPC.h"
#include "QuaternionPC.h"
#include "Vector3PC.h"

const RotationMatrix kRoationMatrixIdentity(1.0f, 0.0f, 0.0f,
                                            0.0f, 1.0f, 0.0f,
                                            0.0f, 0.0f, 1.0f);

void RotationMatrix::identity() {
    m = DirectX::XMFLOAT3X3(1.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 1.0f);
}

// 根据欧拉角设置旋转矩阵
void RotationMatrix::setup(const EulerAngles& orientation) {
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationRollPitchYaw(
        orientation.pitch, orientation.yaw, orientation.roll);
    XMStoreFloat3x3(&m, rotationMat);
}

// 根据惯性-物体四元数设置旋转矩阵
void RotationMatrix::fromInertialToObjectQuaternion(const Quaternion& q) {
    DirectX::XMVECTOR quat = XMLoadFloat4(&q.q);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationQuaternion(quat);
    XMStoreFloat3x3(&m, rotationMat);
}

// 根据物体-惯性四元数设置旋转矩阵
void RotationMatrix::fromObjectToInertialQuaternion(const Quaternion& q) {
    DirectX::XMVECTOR quat = XMLoadFloat4(&q.q);
    DirectX::XMMATRIX rotationMat = DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionInverse(quat));
    XMStoreFloat3x3(&m, rotationMat);
}

// 向量从惯性坐标系到物体坐标系
Vector3 RotationMatrix::inertialToObject(const Vector3& v) const {
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&v.v);
    DirectX::XMMATRIX mat = XMLoadFloat3x3(&m);
    DirectX::XMVECTOR resultVec = XMVector3TransformCoord(vec, mat);
    Vector3 result;
    DirectX::XMStoreFloat3(&result.v, resultVec);
    return result;
}

// 向量从物体坐标系到惯性坐标系
Vector3 RotationMatrix::objectToInertial(const Vector3& v) const {
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&v.v);
    DirectX::XMMATRIX mat = XMMatrixTranspose(XMLoadFloat3x3(&m));
    DirectX::XMVECTOR resultVec = XMVector3TransformCoord(vec, mat);
    Vector3 result;
    DirectX::XMStoreFloat3(&result.v, resultVec);
    return result;
}
#endif
