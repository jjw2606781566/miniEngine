#ifdef WIN32
#include "QuaternionPC.h"
#include "Vector3PC.h"
#include "EulerAnglesPC.h"
#include "Matrix3x4PC.h"
#include "Matrix4x4PC.h"
#include "Engine/math/MathUtils.h"


// Empty Quaternion
const Quaternion kQuaternionEmpty(0.0f, 0.0f, 0.0f, 1.0f);

// 绕轴旋转
void Quaternion::setToRotateAboutX(float theta)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(theta, 0.0f, 0.0f);
	DirectX::XMStoreFloat4(&q, quat);
}

void Quaternion::setToTotateAboutY(float theta)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, theta, 0.0f);
	DirectX::XMStoreFloat4(&q, quat);
}

void Quaternion::setToTotateAboutZ(float theta)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, theta);
	DirectX::XMStoreFloat4(&q, quat);
}

void Quaternion::setToRotateAboutAxis(const Vector3& axis, float theta)
{
	DirectX::XMVECTOR axisVec = DirectX::XMLoadFloat3(&axis.v);
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationAxis(axisVec, theta);
	DirectX::XMStoreFloat4(&q, quat);
}

// 执行物体-惯性坐标系旋转的四元数
void Quaternion::setQuaternionRotationRollPitchYaw(const Vector3& eularAngle)
{
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(
		eularAngle.v.x, eularAngle.v.y, eularAngle.v.z);
	DirectX::XMQuaternionIdentity();
	DirectX::XMStoreFloat4(&q, quat);
}

// 叉乘
Quaternion Quaternion::operator *(const Quaternion& a) const
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&q);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&a.q);
	DirectX::XMVECTOR resultQuat = DirectX::XMQuaternionMultiply(q1, q2);
	Quaternion result;
	DirectX::XMStoreFloat4(&result.q, resultQuat);
	return result;
}

Quaternion Quaternion::operator *=(const Quaternion& a)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&q);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&a.q);
	DirectX::XMVECTOR resultQuat = DirectX::XMQuaternionMultiply(q1, q2);
	DirectX::XMStoreFloat4(&q, resultQuat);
	return *this;
}

void Quaternion::QuaternionRotateVector(Vector3& vec) const
{
	DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&q);
	DirectX::XMMATRIX matrix = DirectX::XMMatrixRotationQuaternion(quaternion);
	DirectX::XMVECTOR vector = DirectX::XMLoadFloat3(&vec.v);
	vector = DirectX::XMVector3Transform(vector, matrix);
	DirectX::XMStoreFloat3(&vec.v, vector);
}

Quaternion Quaternion::getInverse() const
{
	DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&q);
	quaternion = DirectX::XMQuaternionInverse(quaternion);
	Quaternion inverse;
	DirectX::XMStoreFloat4(&(inverse.q), quaternion);
	return inverse;
}


// 正则化四元数
void Quaternion::normalize()
{
	DirectX::XMVECTOR quat = DirectX::XMLoadFloat4(&q);
	quat = DirectX::XMQuaternionNormalize(quat);
	DirectX::XMStoreFloat4(&q, quat);
}

// 提取旋转角度
float Quaternion::getRotationAngle() const
{
	DirectX::XMVECTOR quat = DirectX::XMLoadFloat4(&q);
	float angle;
	DirectX::XMVECTOR axis;
	DirectX::XMQuaternionToAxisAngle(&axis, &angle, quat);
	return angle;
}

// 提取旋转轴
Vector3 Quaternion::getRotationAxis() const
{
	DirectX::XMVECTOR quat = DirectX::XMLoadFloat4(&q);
	float angle;
	DirectX::XMVECTOR axis;
	DirectX::XMQuaternionToAxisAngle(&axis, &angle, quat);
	Vector3 result;
	DirectX::XMStoreFloat3(&result.v, axis);
	return result;
}

Vector3 Quaternion::getEulerAnglesDegree() const
{
	Vector3 eulerAngles;
	DirectX::XMVECTOR vq = DirectX::XMLoadFloat4(&q);
	vq = DirectX::XMQuaternionNormalize(vq);

	// 将四元数转换为矩阵
	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationQuaternion(vq);

	// 提取欧拉角
	if (rotationMatrix.r[2].m128_f32[1] < 0.999f)
	{
		if (rotationMatrix.r[2].m128_f32[1] > -0.999f)
		{
			eulerAngles.v.x = asin(rotationMatrix.r[2].m128_f32[1]);
			eulerAngles.v.y = atan2(rotationMatrix.r[2].m128_f32[0], rotationMatrix.r[2].m128_f32[2]);
			eulerAngles.v.z = atan2(rotationMatrix.r[0].m128_f32[1], rotationMatrix.r[1].m128_f32[1]);
			Vector3::Abs(eulerAngles);
		}
		else
		{
			eulerAngles.v.x = MathUtils::PI * 0.5F;
			eulerAngles.v.y = atan2(rotationMatrix.r[1].m128_f32[0], rotationMatrix.r[0].m128_f32[0]);
			eulerAngles.v.z = 0.0F;
			Vector3::Abs(eulerAngles);
		}
	}
	else
	{
		eulerAngles.v.x = -MathUtils::PI * 0.5F;
		eulerAngles.v.y = atan2(-rotationMatrix.r[1].m128_f32[0],rotationMatrix.r[0].m128_f32[0]);
		eulerAngles.v.z = 0.0F;
		Vector3::Abs(eulerAngles);
	}

	return eulerAngles* MathUtils::RAD_TO_DEG;
}

rapidxml::xml_node<>* Quaternion::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
	auto mXmlNode = doc->allocate_node(rapidxml::node_element, "Quaternion");
	father->append_node(mXmlNode);

	mXmlNode->value(doc->allocate_string(value.c_str()));
	
	mXmlNode->append_attribute(doc->allocate_attribute("x", doc->allocate_string(std::to_string(q.x).c_str())));
	mXmlNode->append_attribute(doc->allocate_attribute("y", doc->allocate_string(std::to_string(q.y).c_str())));
	mXmlNode->append_attribute(doc->allocate_attribute("z", doc->allocate_string(std::to_string(q.z).c_str())));
	mXmlNode->append_attribute(doc->allocate_attribute("w", doc->allocate_string(std::to_string(q.w).c_str())));
	return mXmlNode;
}

void Quaternion::deSerialize(const rapidxml::xml_node<>* node)
{
	q.x = std::stof(node->first_attribute("x")->value());
	q.y = std::stof(node->first_attribute("y")->value());
	q.z = std::stof(node->first_attribute("z")->value());
	q.w = std::stof(node->first_attribute("w")->value());
}


// 四元数的点乘
float dotProduct(const Quaternion& quaternion_a, const Quaternion& quaternion_b)
{
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&quaternion_a.q);
	DirectX::XMVECTOR q2 = DirectX::XMLoadFloat4(&quaternion_b.q);
	return DirectX::XMVectorGetX(DirectX::XMVector4Dot(q1, q2));
}

// 四元数的球面线性插值
Quaternion slerp(const Quaternion& quaternion0, const Quaternion& quaternion1, float t)
{
	DirectX::XMVECTOR q0 = DirectX::XMLoadFloat4(&quaternion0.q);
	DirectX::XMVECTOR q1 = DirectX::XMLoadFloat4(&quaternion1.q);
	DirectX::XMVECTOR resultQuat = DirectX::XMQuaternionSlerp(q0, q1, t);
	Quaternion result;
	DirectX::XMStoreFloat4(&result.q, resultQuat);
	return result;
}

// 四元数的共轭
Quaternion conjugater(const Quaternion& quaternion)
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&quaternion.q);
	DirectX::XMVECTOR resultQuat = DirectX::XMQuaternionConjugate(q);
	Quaternion result;
	DirectX::XMStoreFloat4(&result.q, resultQuat);
	return result;
}


// 四元数的幂
Quaternion pow(const Quaternion& quaternion, float exponent)
{
	DirectX::XMVECTOR q = DirectX::XMLoadFloat4(&quaternion.q);

	// 提取旋转角度和轴
	float angle;
	DirectX::XMVECTOR axis;
	DirectX::XMQuaternionToAxisAngle(&axis, &angle, q);
   
	// 计算新的旋转角度
	float newAngle = angle * exponent;
    
	// 计算新的四元数
	DirectX::XMVECTOR resultQuat = DirectX::XMQuaternionRotationAxis(axis, newAngle);
	Quaternion result;
	DirectX::XMStoreFloat4(&result.q, resultQuat);
	return result;
}

#endif