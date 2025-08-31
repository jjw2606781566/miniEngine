#ifdef WIN32
#include "Vector3PC.h"
#include "Matrix4x4PC.h"
#include "Engine/math/MathUtils.h"


const Vector3 kZeroVector3(0, 0, 0);

float Vector3::Length(const Vector3& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat3(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(xmVec));
}

float Vector3::LengthSquared(const Vector3& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat3(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(xmVec));
}

float Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat3(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat3(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(xmVec1, xmVec2));
}

float Vector3::Distance(const Vector3& v1, const Vector3& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat3(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat3(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(xmVec1, xmVec2)));
}

float Vector3::Angle(const Vector3& v1, const Vector3& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat3(&v1.v);
    xmVec1 = DirectX::XMVector3Normalize(xmVec1);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat3(&v2.v);
    xmVec2 = DirectX::XMVector3Normalize(xmVec2);
    float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(xmVec1, xmVec2));
    //avoid zero direction
    dot = std::max(-1.0f, std::min(1.0f, dot));
    return acosf(dot);
}

Vector3 Vector3::InverseSafe(const Vector3& v)
{
    return Vector3{MathUtils::inverseSafe(v.v.x), MathUtils::inverseSafe(v.v.y), MathUtils::inverseSafe(v.v.z)};
}

Vector3 Vector3::Lerp(const Vector3& start, const Vector3& end, float t)
{
    if (t >= 1)
    {
        return end;
    }
    else if(t < 0)
    {
        return start;
    }
    return Vector3{
        (1 - t) * start.v.x + t * end.v.x,
        (1 - t) * start.v.y + t * end.v.y,
        (1 - t) * start.v.z + t * end.v.z
    };
}

Vector3 Vector3::Abs(const Vector3& input)
{
    return Vector3{ std::abs(input.v.x), std::abs(input.v.y), std::abs(input.v.z) };
}

void Vector3::Normalize(Vector3& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat3(&v.v);
    xmVec = DirectX::XMVector3Normalize(xmVec);
    XMStoreFloat3(&v.v, xmVec);
}

Vector3 Vector3::CrossProduct(const Vector3& v1, const Vector3& v2) {
    DirectX::XMVECTOR xmVec1 = XMLoadFloat3(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat3(&v2.v);
    DirectX::XMVECTOR crossProduct = DirectX::XMVector3Cross(xmVec1, xmVec2);

    Vector3 result;
    XMStoreFloat3(&result.v, crossProduct);
    return result;
}

Vector3 Vector3::CrossProduct(const Vector3& rhs) const {
    return CrossProduct(*this, rhs);
}

void Vector3::TransformSelfToScreen(const Matrix4x4& mat)
{
    DirectX::XMVECTOR vec = DirectX::XMVectorSet(v.x, v.y, v.z, 1.0f);
    DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&(mat.m));
    vec = DirectX::XMVector4Transform(vec, m);
    //perform perspective divide
    if (DirectX::XMVectorGetW(vec) < 0.001f)
    {
        // this point is behind the camera, return null
        v.x = D3D12_FLOAT32_MAX;
        v.y = D3D12_FLOAT32_MAX;
        v.z = D3D12_FLOAT32_MAX;
        return;
    }
    vec = DirectX::XMVectorDivide(vec, DirectX::XMVectorSplatW(vec));
    DirectX::XMStoreFloat3(&v, vec);
}

float Vector3::Length() const
{
    return Length(*this);
}

float Vector3::LengthSquared() const
{
    return LengthSquared(*this);
}

float Vector3::Dot(const Vector3& rhs) const
{
    return Dot(*this, rhs);
}

void Vector3::Scale(const Vector3& rhs)
{
    v.x *= rhs.v.x;
    v.y *= rhs.v.y;
    v.z *= rhs.v.z;
}

Vector3 Vector3::Normalize() const
{
    Vector3 result = *this;
    Normalize(result);
    return result;
}

float Vector3::operator[](int index) const
{
    assert(index >= 0 && index < 3);
    if (index == 0) return v.x;
    if (index == 1) return v.y;
    return v.z;
}


Vector3 Vector3::operator*(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, scalar);
    Vector3 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector3 Vector3::operator/(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, 1.0f / scalar);
    Vector3 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector3 Vector3::operator+(const Vector3& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    Vector3 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector3 Vector3::operator-(const Vector3& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    Vector3 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector3& Vector3::operator+=(const Vector3& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector3& Vector3::operator-=(const Vector3& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}



Vector3& Vector3::operator*=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector3& Vector3::operator/=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, 1.0f / scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

bool Vector3::operator==(const Vector3& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    return DirectX::XMVector3Equal(lhsVec, rhsVec);
}

bool Vector3::operator!=(const Vector3& rhs) const
{
    return !(*this == rhs);
}

DirectX::XMVECTOR Vector3::ToXMVECTOR() const {
    return DirectX::XMLoadFloat3(&v);
}

void Vector3::FromXMVECTOR(const DirectX::XMVECTOR& vec) {
    DirectX::XMStoreFloat3(&v, vec);
}

rapidxml::xml_node<>* Vector3::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    auto mXmlNode = doc->allocate_node(rapidxml::node_element, "Vector3");
    father->append_node(mXmlNode);

    mXmlNode->value(doc->allocate_string(value.c_str()));
    
    mXmlNode->append_attribute(doc->allocate_attribute("x", doc->allocate_string(std::to_string(v.x).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("y", doc->allocate_string(std::to_string(v.y).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("z", doc->allocate_string(std::to_string(v.z).c_str())));
    return mXmlNode;
}

void Vector3::deSerialize(const rapidxml::xml_node<>* node)
{
    v.x = std::stof(node->first_attribute("x")->value());
    v.y = std::stof(node->first_attribute("y")->value());
    v.z = std::stof(node->first_attribute("z")->value());
}

#endif
