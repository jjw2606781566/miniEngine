#include "SphereShape.h"

#include "BoxShape.h"
#include "Engine/math/math.h"

SphereShape::SphereShape(float radius) : radius(radius)
{
    volume = (4.0f / 3.0f) * MathUtils::kPi * radius * radius * radius;
    position = Vector3(0, 0, 0);
}

ShapeType SphereShape::getType() const
{
    return ShapeType::Sphere;
}

bool SphereShape::isConvex() const
{
    return true;
}

void SphereShape::getAABB(const Vector3& position, Vector3& min, Vector3& max) const
{
    min.v.x = position.v.x - radius;
    min.v.y = position.v.y - radius;
    min.v.z = position.v.z - radius;

    max.v.x = position.v.x + radius;
    max.v.y = position.v.y + radius;
    max.v.z = position.v.z + radius;
}

bool SphereShape::localIsInside(const Vector3& point) const
{
    float disSquared = (point.v.x - position.v.x) * (point.v.x - position.v.x) +
                            (point.v.y - position.v.y) * (point.v.y - position.v.y) +
                            (point.v.z - position.v.z) * (point.v.z - position.v.z);
    return disSquared <= radius * radius;
}

void SphereShape::project(const Vector3& position, const Vector3& axis, float& minProj,
                          float& maxProj, Vector3& minPoint, Vector3& maxPoint) const
{
    float centerProj = position.Dot(axis);
    minProj = centerProj - radius;
    maxProj = centerProj + radius;
    minPoint = position - axis * radius;
    maxPoint = position + axis * radius;
}

float SphereShape::getRadius() const
{
    return radius;
}




bool SphereShape::checkCollision(const BaseShape& other) const
{
    if (other.getType() == ShapeType::Sphere)
    {
        const SphereShape& otherSphere = static_cast<const SphereShape&>(other);
        Vector3 distance = otherSphere.getPosition() - getPosition();
        float radiiSum = getRadius() + otherSphere.getRadius();
        
        // std::cout << "distance: " << distance.v.x << "," << distance.v.y << "," << distance.v.z <<
        //     ", radius: " << radiiSum << std::endl;
        
        return distance.Length() <= radiiSum;
    }
    if (other.getType() == ShapeType::Box)
    {
        const BoxShape& box = static_cast<const BoxShape&>(other);
        Vector3 closestPoint;
        Vector3 spherePos = getPosition();
        Vector3 boxMin, boxMax;
        box.getAABB(box.getPosition(), boxMin, boxMax);

        closestPoint.v.x = (spherePos.v.x < boxMin.v.x) ? boxMin.v.x : 
                   (spherePos.v.x > boxMax.v.x) ? boxMax.v.x : spherePos.v.x;
        closestPoint.v.y = (spherePos.v.y < boxMin.v.y) ? boxMin.v.y : 
                           (spherePos.v.y > boxMax.v.y) ? boxMax.v.y : spherePos.v.y;
        closestPoint.v.z = (spherePos.v.z < boxMin.v.z) ? boxMin.v.z : 
                           (spherePos.v.z > boxMax.v.z) ? boxMax.v.z : spherePos.v.z;

        Vector3 distance = closestPoint - spherePos;
        // std::cout << "closestPoint: " << closestPoint.v.x << "," << closestPoint.v.y << "," << closestPoint.v.z <<
        //     ", spherePos: " << spherePos.v.x << "," << spherePos.v.y << "," << spherePos.v.z << std::endl;
        // std::cout << "distance: " << distance.v.x << "," << distance.v.y << "," << distance.v.z <<
        //     ", radius: " << getRadius() << std::endl;
        return distance.Length() <= getRadius();
    } 
    return false;
}

bool SphereShape::rayIntersect(const Vector3& origin, const Vector3& direction, float maxDistance, float& t,
    Vector3& normal) const
{
    Vector3 L = getPosition() - origin;
    float tca = Vector3::Dot(L, direction);
    if (tca < 0) return false;
        
    float d2 = Vector3::Dot(L, L) - tca * tca;
    float radius2 = getRadius() * getRadius();
    if (d2 > radius2) return false;

    float thc = sqrt(radius2 - d2);
    t = tca - thc;

    if (t < 0 || t > maxDistance) return false;
        
    Vector3 hitPoint = origin + direction * t;
    normal = (hitPoint - getPosition()).Normalize();
    return true;
}

rapidxml::xml_node<>* SphereShape::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "SphereShape");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("radius", doc->allocate_string(std::to_string(radius).c_str())));
    return mXmlNode;
}

void SphereShape::deSerialize(const rapidxml::xml_node<>* node)
{
    radius = std::stof(node->first_attribute("radius")->value());
}
