
#include "Engine/Memory/TankinMemory.h"
#ifdef WIN32
#include "Vector2PC.h"

const Vector2 kZeroVector2( 0.0f, 0.0f );

float Vector2::Length(const Vector2& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat2(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector2Length(xmVec));
}

float Vector2::LengthSquared(const Vector2& v)
{
    DirectX::XMVECTOR xmVec = XMLoadFloat2(&v.v);
    return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(xmVec));
}

float Vector2::Dot(const Vector2& v1, const Vector2& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat2(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat2(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector2Dot(xmVec1, xmVec2));
}

float Vector2::Distance(const Vector2& v1, const Vector2& v2)
{
    DirectX::XMVECTOR xmVec1 = XMLoadFloat2(&v1.v);
    DirectX::XMVECTOR xmVec2 = XMLoadFloat2(&v2.v);
    return DirectX::XMVectorGetX(DirectX::XMVector2Length(DirectX::XMVectorSubtract(xmVec1, xmVec2)));
}

void Vector2::Normalize(Vector2& v)
{
    DirectX::XMVECTOR xmVec = DirectX::XMLoadFloat2(&v.v);
    xmVec = DirectX::XMVector2Normalize(xmVec);
    DirectX::XMStoreFloat2(&v.v, xmVec);
}

float Vector2::CrossProduct(const Vector2& v1, const Vector2& v2)
{
    DirectX::XMVECTOR xmVec1 = DirectX::XMLoadFloat2(&v1.v);
    DirectX::XMVECTOR xmVec2 = DirectX::XMLoadFloat2(&v2.v);

    // 计算叉积
    float crossProduct = DirectX::XMVectorGetX(xmVec1) * DirectX::XMVectorGetY(xmVec2) -
                         DirectX::XMVectorGetY(xmVec1) * DirectX::XMVectorGetX(xmVec2);

    return crossProduct;
}


float Vector2::Length() const
{
    return Length(*this);
}

float Vector2::LengthSquared() const
{
    return LengthSquared(*this);
}

float Vector2::Dot(const Vector2& rhs) const
{
    return Dot(*this, rhs);
}

Vector2 Vector2::Normalize() const
{
    Vector2 result = *this;
    Normalize(result);
    return result;
}

float Vector2::CrossProduct(const Vector2& rhs) const
{
    DirectX::XMVECTOR xmVec1 = DirectX::XMLoadFloat2(&this->v);
    DirectX::XMVECTOR xmVec2 = DirectX::XMLoadFloat2(&rhs.v);

    // 提取向量分量
    float x1 = DirectX::XMVectorGetX(xmVec1);
    float y1 = DirectX::XMVectorGetY(xmVec1);
    float x2 = DirectX::XMVectorGetX(xmVec2);
    float y2 = DirectX::XMVectorGetY(xmVec2);

    // 计算叉积
    return (x1 * y2) - (y1 * x2);
}


float Vector2::operator[](int index) const
{
    return index == 0 ? v.x : v.y;
}

Vector2 Vector2::operator*(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, scalar);
    Vector2 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector2 Vector2::operator/(float scalar) const
{
    DirectX::XMVECTOR vec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(vec, 1.0f / scalar);
    Vector2 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector2 Vector2::operator+(const Vector2& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    Vector2 result;
    result.FromXMVECTOR(resultVec);
    return result;
}

Vector2 Vector2::operator-(const Vector2& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    Vector2 result;
    result.FromXMVECTOR(resultVec);
    return result;
}


Vector2& Vector2::operator+=(const Vector2& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorAdd(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector2& Vector2::operator-=(const Vector2& rhs)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorSubtract(lhsVec, rhsVec);
    this->FromXMVECTOR(resultVec);
    return *this;
}


Vector2& Vector2::operator*=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

Vector2& Vector2::operator/=(float scalar)
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR resultVec = DirectX::XMVectorScale(lhsVec, 1.0f / scalar);
    this->FromXMVECTOR(resultVec);
    return *this;
}

bool Vector2::operator==(const Vector2& rhs) const
{
    DirectX::XMVECTOR lhsVec = this->ToXMVECTOR();
    DirectX::XMVECTOR rhsVec = rhs.ToXMVECTOR();
    return DirectX::XMVector2Equal(lhsVec, rhsVec);
}

bool Vector2::operator!=(const Vector2& rhs) const
{
    return !(*this == rhs);
}

DirectX::XMVECTOR Vector2::ToXMVECTOR() const {
    return XMLoadFloat2(&v);
}

void Vector2::FromXMVECTOR(const DirectX::XMVECTOR& vec) {
    XMStoreFloat2(&v, vec);
}

rapidxml::xml_node<>* Vector2::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    auto mXmlNode = doc->allocate_node(rapidxml::node_element,"Vector2");
    father->append_node(mXmlNode);

    mXmlNode->value(doc->allocate_string(value.c_str()));
    
    mXmlNode->append_attribute(doc->allocate_attribute("x", doc->allocate_string(std::to_string(v.x).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("y", doc->allocate_string(std::to_string(v.y).c_str())));
    return mXmlNode;
}

void Vector2::deSerialize(const rapidxml::xml_node<>* node)
{
    v.x = std::stof(node->first_attribute("x")->value());
    v.y = std::stof(node->first_attribute("y")->value());
}

Vector2 Vector2::Abs(const Vector2& input)
{
    return Vector2{ std::abs(input.v.x), std::abs(input.v.y) };
}

#endif


