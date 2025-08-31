#ifdef WIN32
#include "Matrix4x4PC.h"

#include "EulerAnglesPC.h"
#include "QuaternionPC.h"
#include "RotationMatrixPC.h"
#include "Vector3PC.h"

const Matrix4x4 Matrix4x4::Identity = { 1, 0, 0, 0,
                                        0, 1, 0, 0,
                                        0, 0, 1, 0,
                                        0, 0, 0, 1};

const Matrix4x4 Matrix4x4::Zero =     { 0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};


void Matrix4x4::setTranslation(float tx, float ty, float tz) {
     DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(tx, ty, tz);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(translation));
}

void Matrix4x4::setRotationX(float theta) {
    DirectX::XMMATRIX rotationX = DirectX::XMMatrixRotationX(theta);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(rotationX));
}

void Matrix4x4::setRotationY(float theta) {
    DirectX::XMMATRIX rotationY = DirectX::XMMatrixRotationY(theta);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(rotationY));
}

void Matrix4x4::setRotationZ(float theta) {
    DirectX::XMMATRIX rotationZ = DirectX::XMMatrixRotationZ(theta);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(rotationZ));
}

void Matrix4x4::setScale(float sx, float sy, float sz) {
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(sx, sy, sz);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(scale));
}


void Matrix4x4::setPerspectiveProjection(float fov, float aspect, float zn, float zf) {
    DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, zn, zf);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(projection));
}

void Matrix4x4::setOrthographicProjection(float width, float height, float zn, float zf)
{
    DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicLH(width, height, zn, zf);
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(projection));
}

void Matrix4x4::setView(const Vector3& position, const Vector3& forward, const Vector3& up, const Vector3& right)
{
    DirectX::XMVECTOR XMposition = DirectX::XMLoadFloat3(&position.v);
    DirectX::XMVECTOR XMforward = DirectX::XMLoadFloat3(&forward.v);
    DirectX::XMVECTOR XMup = DirectX::XMLoadFloat3(&up.v);
    DirectX::XMVECTOR XMright = DirectX::XMLoadFloat3(&right.v);

    XMforward = DirectX::XMVector3Normalize(XMforward);
    XMup = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(XMforward, XMright));
    
    DirectX::XMMATRIX view = DirectX::XMMatrixLookToLH(XMposition,XMforward,XMup);
    view = DirectX::XMMatrixTranspose(view);
    XMStoreFloat4x4(&m, view);
}

void Matrix4x4::lookAt(const Vector3& positon, const Vector3& target, const Vector3& up)
{
    DirectX::XMVECTOR XMposition = DirectX::XMLoadFloat3(&positon.v);
    DirectX::XMVECTOR XMtarget = DirectX::XMLoadFloat3(&target.v);
    DirectX::XMVECTOR XMup = DirectX::XMLoadFloat3(&up.v);
    DirectX::XMMATRIX view = DirectX::XMMatrixLookAtLH(XMposition,XMtarget,XMup);
    XMStoreFloat4x4(&m, DirectX::XMMatrixTranspose(view));
}

void Matrix4x4::setModelMatrix(const Vector3& position,const Vector3& rotation,const Vector3& scale)
{
    DirectX::XMVECTOR XMRotationOrigin = DirectX::XMVectorZero();
    DirectX::XMVECTOR XMposition = DirectX::XMLoadFloat3(&position.v);
    DirectX::XMVECTOR XMrotation = DirectX::XMQuaternionRotationRollPitchYaw(rotation.v.x, rotation.v.y, rotation.v.z);
    DirectX::XMVECTOR XMscale = DirectX::XMLoadFloat3(&scale.v);

    DirectX::XMMATRIX model = DirectX::XMMatrixAffineTransformation(
        XMscale,XMRotationOrigin,XMrotation,XMposition);
    model = DirectX::XMMatrixTranspose(model);
    XMStoreFloat4x4(&m, model);
}

void Matrix4x4::setModelMatrixQuaternion(const Vector3& position, const Quaternion& quaternion, const Vector3& scale)
{
    DirectX::XMVECTOR XMRotationOrigin = DirectX::XMVectorZero();
    DirectX::XMVECTOR XMposition = DirectX::XMLoadFloat3(&position.v);
    DirectX::XMVECTOR XMrotation = DirectX::XMLoadFloat4(&quaternion.q);
    DirectX::XMVECTOR XMscale = DirectX::XMLoadFloat3(&scale.v);

    DirectX::XMMATRIX model = DirectX::XMMatrixAffineTransformation(
        XMscale,XMRotationOrigin,XMrotation,XMposition);
    model = DirectX::XMMatrixTranspose(model);
    XMStoreFloat4x4(&m, model);
}

void Matrix4x4::inverseSelf()
{
    DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&m);
    matrix = DirectX::XMMatrixInverse(nullptr, matrix);
    DirectX::XMStoreFloat4x4(&m, matrix);
}

Matrix4x4 Matrix4x4::transpose() const
{
    DirectX::XMMATRIX mat = DirectX::XMLoadFloat4x4(&m);
    mat = DirectX::XMMatrixTranspose(mat);
    Matrix4x4 ret;
    DirectX::XMStoreFloat4x4(&ret.m, mat);
    return ret;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
    DirectX::XMMATRIX left = DirectX::XMLoadFloat4x4(&m);
    DirectX::XMMATRIX right = DirectX::XMLoadFloat4x4(&other.m);
    DirectX::XMMATRIX result = left * right;
    Matrix4x4 ret;
    DirectX::XMStoreFloat4x4(&ret.m, result);
    return ret;
}
#endif
