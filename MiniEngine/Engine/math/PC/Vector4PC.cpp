#include "Vector4PC.h"

#include "Matrix4x4PC.h"
#include "Engine/math/MathUtils.h"

float Vector4::Length(const Vector4& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat4(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector4Length(xmVec));
}

float Vector4::LengthSquared(const Vector4& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat4(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(xmVec));
}

float Vector4::Dot(const Vector4& v1, const Vector4& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat4(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat4(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector4Dot(xmVec1, xmVec2));
}

float Vector4::Distance(const Vector4& v1, const Vector4& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat4(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat4(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector4Length(DirectX::XMVectorSubtract(xmVec1, xmVec2)));
}

float Vector4::Angle(const Vector4& v1, const Vector4& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat4(&v1.v);
    xmVec1 = DirectX::XMVector4Normalize(xmVec1);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat4(&v2.v);
    xmVec2 = DirectX::XMVector4Normalize(xmVec2);
    float dot = DirectX::XMVectorGetX(DirectX::XMVector4Dot(xmVec1, xmVec2));
    //avoid zero direction
    dot = std::max(-1.0f, std::min(1.0f, dot));
    return acosf(dot);
}

void Vector4::Normalize(Vector4& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat4(&v.v);
    xmVec = DirectX::XMVector4Normalize(xmVec);
    XMStoreFloat4(&v.v, xmVec);
}

Vector4 Vector4::CrossProduct(const Vector4& v1, const Vector4& v2, const Vector4& v3)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat4(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat4(&v2.v);
    DirectX::XMVECTOR xmVec3 = XMLoadFloat4(&v3.v);
    DirectX::XMVECTOR crossProduct = DirectX::XMVector4Cross(xmVec1, xmVec2, xmVec3);

    Vector4 result;
    XMStoreFloat4(&result.v, crossProduct);
    return result;
}

Vector4 Vector4::InverseSafe(const Vector4& v)
{
    return Vector4{MathUtils::inverseSafe(v.v.x), MathUtils::inverseSafe(v.v.y), MathUtils::inverseSafe(v.v.z), MathUtils::inverseSafe(v.v.w)};
}

Vector4 Vector4::Lerp(const Vector4& start, const Vector4& end, float t)
{
    DirectX::XMVECTOR startVec = DirectX::XMLoadFloat4(&start.v);
    DirectX::XMVECTOR endVec = DirectX::XMLoadFloat4(&end.v);
    DirectX::XMVECTOR lerpVec = DirectX::XMVectorLerp(startVec, endVec, t);
    
    Vector4 result;
    DirectX::XMStoreFloat4(&result.v, lerpVec);
    return result;
}

Vector4 Vector4::Abs(const Vector4& input)
{
    return Vector4{ std::abs(input.v.x), std::abs(input.v.y), std::abs(input.v.z), std::abs(input.v.w)};
}

float Vector4::Length() const
{
    return Length(*this);
}

float Vector4::LengthSquared() const
{
    return LengthSquared(*this);
}

float Vector4::Dot(const Vector4& rhs) const
{
    return Dot(*this, rhs);
}

void Vector4::Scale(const Vector4& rhs)
{
    v.x *= rhs.v.x;
    v.y *= rhs.v.y;
    v.z *= rhs.v.z;
    v.w *= rhs.v.w;
}

Vector4 Vector4::Normalize() const
{
    Vector4 result = *this;
    Normalize(result);
    return result;
}

Vector4 Vector4::CrossProduct(const Vector4& rhs, const Vector4 rlhs) const
{
    return CrossProduct(*this, rhs, rlhs);
}

void Vector4::TransformSelfToScreen(const Matrix4x4& mat)
{
    DirectX::XMVECTOR vec = ToXMVECTOR(); 
    DirectX::XMMATRIX m = DirectX::XMLoadFloat4x4(&(mat.m));

    vec = DirectX::XMVector4Transform(vec, m);

    // 获取变换后w分量
    float w = DirectX::XMVectorGetW(vec);

    if (w < 0.001f)
    {
        v.x = D3D12_FLOAT32_MAX;
        v.y = D3D12_FLOAT32_MAX;
        v.z = D3D12_FLOAT32_MAX;
        v.w = D3D12_FLOAT32_MAX;
        return;
    }
    
    vec = DirectX::XMVectorDivide(vec, DirectX::XMVectorSplatW(vec));
    FromXMVECTOR(vec);
}

float Vector4::operator[](int index) const
{
    assert(index >= 0 && index < 4);
    if (index == 0) return v.x;
    if (index == 1) return v.y;
    if (index == 2) return v.z;
    return v.w;
}

Vector4 Vector4::operator*(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, scalar);
    Vector4 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector4 Vector4::operator/(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, 1.0f / scalar);
    Vector4 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector4 Vector4::operator+(const Vector4& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    Vector4 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector4 Vector4::operator-(const Vector4& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    Vector4 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector4& Vector4::operator+=(const Vector4& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector4& Vector4::operator-=(const Vector4& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector4& Vector4::operator*=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector4& Vector4::operator/=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, 1.0f / scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

bool Vector4::operator==(const Vector4& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    return DirectX::XMVector3Equal(lhsVec, rhsVec);
}

bool Vector4::operator!=(const Vector4& rhs) const
{
    return !(*this == rhs);
}

DirectX::XMVECTOR Vector4::ToXMVECTOR() const
{
    return DirectX::XMLoadFloat4(&v); 
}

void Vector4::FromXMVECTOR(const DirectX::XMVECTOR& vec)
{
    DirectX::XMStoreFloat4(&v, vec);
}

rapidxml::xml_node<>* Vector4::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    auto mXmlNode = doc->allocate_node(rapidxml::node_element, "Vector4");
    father->append_node(mXmlNode);

    mXmlNode->value(doc->allocate_string(value.c_str()));
    
    mXmlNode->append_attribute(doc->allocate_attribute("x", doc->allocate_string(std::to_string(v.x).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("y", doc->allocate_string(std::to_string(v.y).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("z", doc->allocate_string(std::to_string(v.z).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("w", doc->allocate_string(std::to_string(v.z).c_str())));
    return mXmlNode;
}

void Vector4::deSerialize(const rapidxml::xml_node<>* node)
{
    v.x = std::stof(node->first_attribute("x")->value());
    v.y = std::stof(node->first_attribute("y")->value());
    v.z = std::stof(node->first_attribute("z")->value());
    v.w = std::stof(node->first_attribute("w")->value());
}
