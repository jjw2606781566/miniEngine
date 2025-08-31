#ifdef WIN32
#include "Matrix3x4PC.h"
#include "EulerAnglesPC.h"
#include "QuaternionPC.h"
#include "RotationMatrixPC.h"
#include "Vector3PC.h"

void Matrix3x4::identity() {
    m = DirectX::XMFLOAT3X4(1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f);
}

// 将平移部分的第四列置为0
void Matrix3x4::zeroTranslation() {
    m._14 = m._24 = m._34 = 0.0f;
}

// 平移部分赋值, 参数为向量形式
void Matrix3x4::setTranslation(const Vector3& d) {
    m._14 = d.v.x;
    m._24 = d.v.y;
    m._34 = d.v.z;
}

// 平移部分赋值, 参数为向量形式
void Matrix3x4::setupTranslation(const Vector3& d) {
    // 线性变换部分置为单位矩阵
    identity();
    setTranslation(d);
}

// 构造执行局部 -> 父空间变换的矩阵
// 构造物体 -> 世界的变换矩阵
// 方位可以由欧拉角或旋转矩阵指定
void Matrix3x4::setupLocalToParent(const Vector3& pos, const EulerAngles& orient) {
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(orient.pitch, orient.yaw, orient.roll);
    DirectX::XMVECTOR translation = DirectX::XMLoadFloat3(&pos.v);
    DirectX::XMMATRIX transform = DirectX::XMMatrixTranslationFromVector(translation) * rotation;
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(transform));
}

void Matrix3x4::setupLocalToParent(const Vector3& pos, const RotationMatrix& orient) {
    m._11 = orient.m._11; m._12 = orient.m._12;
    m._21 = orient.m._22; m._23 = orient.m._23;
    m._31 = orient.m._31; m._32 = orient.m._32;

    m._14 = pos.v.x; m._24 = pos.v.y; m._34 = pos.v.z;
}

// 构造执行父 -> 局部空间变换的矩阵
// 构造世界 -> 无图的变换矩阵
// 方位可以由欧拉角或旋转矩阵指定
void Matrix3x4::setupParentToLocal(const Vector3& pos, const EulerAngles& orient)
{
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(-orient.pitch, -orient.yaw, -orient.roll);
    DirectX::XMVECTOR translation = DirectX::XMVectorNegate(XMLoadFloat3(&pos.v));
    DirectX::XMMATRIX transform = rotation * DirectX::XMMatrixTranslationFromVector(translation);
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(transform));
}

void Matrix3x4::setupParentToLocal(const Vector3& pos, const RotationMatrix& orient)
{
    m._11 = orient.m._11; m._12 = orient.m._12;
    m._21 = orient.m._22; m._23 = orient.m._23;
    m._31 = orient.m._31; m._32 = orient.m._32;

    m._14 = -(pos.v.x * m._11 + pos.v.y * m._21 + pos.v.z * m._31);
    m._24 = -(pos.v.x * m._12 + pos.v.y * m._22 + pos.v.z * m._32);
    m._34 = -(pos.v.x * m._13 + pos.v.y * m._23 + pos.v.z * m._33);
}

// 构造绕坐标轴旋转的矩阵
// 1 => 绕x轴旋转
// 2 => 绕y轴旋转
// 3 => 绕z轴旋转
void Matrix3x4::setupRotate(int axis, float theta)
{
    DirectX::XMMATRIX rotation;
    switch (axis) {
    case 1: // X-axis
        rotation = DirectX::XMMatrixRotationX(theta);
        break;
    case 2: // Y-axis
        rotation = DirectX::XMMatrixRotationY(theta);
        break;
    case 3: // Z-axis
        rotation = DirectX::XMMatrixRotationZ(theta);
        break;
    default:
        rotation = DirectX::XMMatrixIdentity();
        break;
    }
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(rotation));
}

void Matrix3x4::setupRotation(const Vector3& rotation)
{
    DirectX::XMMATRIX rotationMatrix =
        DirectX::XMMatrixRotationRollPitchYaw(rotation.v.x, rotation.v.y, rotation.v.z);
    rotationMatrix = DirectX::XMMatrixTranspose(rotationMatrix);
    DirectX::XMStoreFloat3x4(&m,rotationMatrix);
}

// 构造绕任意轴旋转
void Matrix3x4::setupRotate(const Vector3& axis, float theta)
{
    DirectX::XMVECTOR axisVec = XMLoadFloat3(&axis.v);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationAxis(axisVec, theta);
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(rotation));
}

// 四元数转换到矩阵
void Matrix3x4::fromQuaternion(const Quaternion& q) {
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&q.q));
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(rotation));
}

// 构造沿各坐标缩放的矩阵
void Matrix3x4::setupScale(const Vector3& s) {
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(s.v.x, s.v.y, s.v.z);
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(scale));
}

// 构造沿任意轴缩放的矩阵
void Matrix3x4::setupScaleAlongAxis(const Vector3& axis, float k) {
    DirectX::XMVECTOR axisVec = DirectX::XMLoadFloat3(&axis.v);
    DirectX::XMVECTOR scaledAxisVec = DirectX::XMVectorScale(axisVec, k);
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScalingFromVector(scaledAxisVec);
    DirectX::XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(scaleMatrix));
}

// 构造切变矩阵
void Matrix3x4::setupShear(int axis, float s, float t)
{
    DirectX::XMMATRIX shearMatrix = DirectX::XMMatrixIdentity();

    switch (axis) {
    case 1: // x 轴上的切变，影响 y 和 z
        shearMatrix.r[1].m128_f32[0] = s; // y = y + s * x
        shearMatrix.r[2].m128_f32[0] = t; // z = z + t * x
        break;
    case 2: // y 轴上的切变，影响 x 和 z
        shearMatrix.r[0].m128_f32[1] = s; // x = x + s * y
        shearMatrix.r[2].m128_f32[1] = t; // z = z + t * y
        break;
    case 3: // z 轴上的切变，影响 x 和 y
        shearMatrix.r[0].m128_f32[2] = s; // x = x + s * z
        shearMatrix.r[1].m128_f32[2] = t; // y = y + t * z
        break;
    default:
        throw std::invalid_argument("Invalid axis for shearing. Must be 1, 2, or 3.");
    }
    
    DirectX::XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(shearMatrix));
}

// 构造投影矩阵，投影平面过原点
void Matrix3x4::setupProject(const Vector3& n)
{
    DirectX::XMVECTOR normal = XMLoadFloat3(&n.v);
    float a = n.v.x;
    float b = n.v.y;
    float c = n.v.z;

    DirectX::XMMATRIX projMatrix = DirectX::XMMatrixIdentity();
    projMatrix.r[0].m128_f32[0] = 1 - a * a;
    projMatrix.r[0].m128_f32[1] = -a * b;
    projMatrix.r[0].m128_f32[2] = -a * c;

    projMatrix.r[1].m128_f32[0] = -a * b;
    projMatrix.r[1].m128_f32[1] = 1 - b * b;
    projMatrix.r[1].m128_f32[2] = -b * c;

    projMatrix.r[2].m128_f32[0] = -a * c;
    projMatrix.r[2].m128_f32[1] = -b * c;
    projMatrix.r[2].m128_f32[2] = 1 - c * c;

    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(projMatrix));
}

// 反射平面由一个索引指定
// 1 => 沿x = k平面反射
// 2 => 沿y = k平面反射
// 3 => 沿z = k平面反射
void Matrix3x4::setupReflect(int axis, float k) {
    DirectX::XMVECTOR planeNormal;
    DirectX::XMVECTOR planePoint;
    
    switch (axis) {
    case 1: // x = k
        planeNormal = DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
        planePoint = DirectX::XMVectorSet(k, 0.0f, 0.0f, 1.0f);
        break;
    case 2: // y = k
        planeNormal = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        planePoint = DirectX::XMVectorSet(0.0f, k, 0.0f, 1.0f);
        break;
    case 3: // z = k
        planeNormal = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
        planePoint = DirectX::XMVectorSet(0.0f, 0.0f, k, 1.0f);
        break;
    default:
            DirectX::XMStoreFloat3x4(&m, DirectX::XMMatrixIdentity());
        return;
    }
    
    DirectX::XMMATRIX reflectMat = DirectX::XMMatrixReflect(DirectX::XMPlaneFromPointNormal(planePoint, planeNormal));
    DirectX::XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(reflectMat));
}

void Matrix3x4::setupReflect(const Vector3& n) {
    DirectX::XMVECTOR normal = XMLoadFloat3(&n.v);
    DirectX::XMMATRIX reflect = DirectX::XMMatrixReflect(normal);
    XMStoreFloat3x4(&m, DirectX::XMMatrixTranspose(reflect));
}


DirectX::XMMATRIX Matrix3x4::ToXMMATRIX() const {
    return DirectX::XMMATRIX(
        m._11, m._12, m._13, m._14,
        m._21, m._22, m._23, m._24,
        m._31, m._32, m._33, m._34,
        0.0f,  0.0f,  0.0f,  1.0f
    );
}

Matrix3x4 Matrix3x4::FromXMMATRIX(const DirectX::XMMATRIX& mat)
{
    return Matrix3x4(
        mat.r[0].m128_f32[0], mat.r[0].m128_f32[1], mat.r[0].m128_f32[2], mat.r[0].m128_f32[3],
        mat.r[1].m128_f32[0], mat.r[1].m128_f32[1], mat.r[1].m128_f32[2], mat.r[1].m128_f32[3],
        mat.r[2].m128_f32[0], mat.r[2].m128_f32[1], mat.r[2].m128_f32[2], mat.r[2].m128_f32[3]
        );
}


// 矩阵与向量相乘
Vector3 operator*(const Matrix3x4& m, const Vector3& v) {
    DirectX::XMVECTOR vec = XMLoadFloat3(&v.v);
    DirectX::XMMATRIX mat = m.ToXMMATRIX();
    DirectX::XMMatrixTranspose(mat);
    DirectX::XMVECTOR transformedVec = XMVector3Transform(vec, mat);
    Vector3 result;
    XMStoreFloat3(&result.v, transformedVec);
    return result;
}

// 矩阵与矩阵相乘
Matrix3x4 operator*(const Matrix3x4& m1, const Matrix3x4& m2) {
    DirectX::XMMATRIX mat1 = m1.ToXMMATRIX();
    DirectX::XMMatrixTranspose(mat1);
    DirectX::XMMATRIX mat2 = m2.ToXMMATRIX();
    DirectX::XMMatrixTranspose(mat2);
    DirectX::XMMATRIX resultMat = XMMatrixMultiply(mat1, mat2);
    return Matrix3x4::FromXMMATRIX(XMMatrixTranspose(resultMat));
}



// 计算矩阵的行列式
float determinant(const Matrix3x4& m) {
    DirectX::XMMATRIX mat = m.ToXMMATRIX();
    DirectX::XMVECTOR det = XMMatrixDeterminant(mat);
    return DirectX::XMVectorGetX(det);
}

// 计算矩阵的逆矩阵
Matrix3x4 inverse(const Matrix3x4& m) {
    DirectX::XMMATRIX mat = m.ToXMMATRIX();
    DirectX::XMVECTOR det;
    DirectX::XMMATRIX invMat = XMMatrixInverse(&det, mat);
    return Matrix3x4::FromXMMATRIX(invMat);
}

// 获取矩阵的平移部分
Vector3 getTranslation(const Matrix3x4& m) {
    return Vector3(m.m._14, m.m._24, m.m._34);
}

// 获取从父到局部的矩阵中的位置
Vector3 getPositionFromParentToLocalMatrix(const Matrix3x4& m) {
    return getTranslation(m);
}

// 获取从局部到父的矩阵中的位置
Vector3 getPositionFromLocalToParentMatrix(const Matrix3x4& m) {
    return getTranslation(m);
}

const Matrix3x4 kMatrixIdentity(
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f);

#endif