#include "BoxShape.h"
#include "SphereShape.h"

BoxShape::BoxShape(const Vector3& size) : mSize(size)
{
    volume = size.v.x * size.v.y * size.v.z;
    position = Vector3(0, 0, 0);
}

ShapeType BoxShape::getType() const
{
    return ShapeType::Box;
}

bool BoxShape::isConvex() const
{
    return true;
}

void BoxShape::getAABB(const Vector3& position, Vector3& min, Vector3& max) const
{
    min.v.x = position.v.x - mSize.v.x / 2.0f;
    min.v.y = position.v.y - mSize.v.y / 2.0f;
    min.v.z = position.v.z - mSize.v.z / 2.0f;

    max.v.x = position.v.x + mSize.v.x / 2.0f;
    max.v.y = position.v.y + mSize.v.y / 2.0f;
    max.v.z = position.v.z + mSize.v.z / 2.0f;
}

bool BoxShape::localIsInside(const Vector3& point) const
{
    return (point.v.x >= position.v.x - mSize.v.x / 2.0f && point.v.x <= position.v.x + mSize.v.x / 2.0f) &&
           (point.v.y >= position.v.y - mSize.v.y / 2.0f && point.v.y <= position.v.y + mSize.v.y / 2.0f) &&
           (point.v.z >= position.v.z - mSize.v.z / 2.0f && point.v.z <= position.v.z + mSize.v.z / 2.0f);
}

void BoxShape::project(const Vector3& position, const Vector3& axis, float& minProj,
                       float& maxProj, Vector3& minPoint, Vector3& maxPoint) const
{
    Vector3 vertices[8] =
    {
        Vector3(position.v.x - mSize.v.x / 2.0f, position.v.y - mSize.v.y / 2.0f, position.v.z - mSize.v.z / 2.0f),
        Vector3(position.v.x + mSize.v.x / 2.0f, position.v.y - mSize.v.y / 2.0f, position.v.z - mSize.v.z / 2.0f),
        Vector3(position.v.x - mSize.v.x / 2.0f, position.v.y + mSize.v.y / 2.0f, position.v.z - mSize.v.z / 2.0f),
        Vector3(position.v.x + mSize.v.x / 2.0f, position.v.y + mSize.v.y / 2.0f, position.v.z - mSize.v.z / 2.0f),
        Vector3(position.v.x - mSize.v.x / 2.0f, position.v.y - mSize.v.y / 2.0f, position.v.z + mSize.v.z / 2.0f),
        Vector3(position.v.x + mSize.v.x / 2.0f, position.v.y - mSize.v.y / 2.0f, position.v.z + mSize.v.z / 2.0f),
        Vector3(position.v.x - mSize.v.x / 2.0f, position.v.y + mSize.v.y / 2.0f, position.v.z + mSize.v.z / 2.0f),
        Vector3(position.v.x + mSize.v.x / 2.0f, position.v.y + mSize.v.y / 2.0f, position.v.z + mSize.v.z / 2.0f)
    };
    
    minProj = largeFloat;
    maxProj = smallFloat;

    for (int i = 0; i < 8; ++i)
    {
        float proj = vertices[i].Dot(axis);
        if (proj < minProj)
        {
            minProj = proj;
            minPoint = vertices[i];
        }
        if (proj > maxProj)
        {
            maxProj = proj;
            maxPoint = vertices[i];
        }
    }
}

Vector3 BoxShape::getSize() const
{
    return mSize;
}

bool BoxShape::checkCollision(const BaseShape& other) const
{
    if (other.getType() == ShapeType::Box)
    {
        const BoxShape& otherBox = static_cast<const BoxShape&>(other);
        Vector3 thisMin, thisMax, otherMin, otherMax;
        getAABB(getPosition(), thisMin, thisMax);
        otherBox.getAABB(otherBox.getPosition(), otherMin, otherMax);

        // std::cout << "thisMin: " << thisMin.v.x << "," << thisMin.v.y << "," << thisMin.v.z <<
        //     ", thisMax: " << thisMax.v.x << "," << thisMax.v.y << "," << thisMax.v.z << std::endl;
        // std::cout << "otherMin: " << otherMin.v.x << "," << otherMin.v.y << "," << otherMin.v.z <<
        //     ", otherMax: " << otherMax.v.x << "," << otherMax.v.y << "," << otherMax.v.z << std::endl;
        
        return (thisMax.v.x >= otherMin.v.x && thisMin.v.x <= otherMax.v.x) &&
               (thisMax.v.y >= otherMin.v.y && thisMin.v.y <= otherMax.v.y) &&
               (thisMax.v.z >= otherMin.v.z && thisMin.v.z <= otherMax.v.z);
    }
    if (other.getType() == ShapeType::Sphere)
    {
        const SphereShape& sphere = static_cast<const SphereShape&>(other);
        Vector3 closestPoint;
        Vector3 spherePos = sphere.getPosition();
        Vector3 boxMin, boxMax;
        getAABB(getPosition(), boxMin, boxMax);
        
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
        //     ", radius: " << sphere.getRadius() << std::endl;
        return distance.Length() <= sphere.getRadius();
    }
    return false;
}

bool BoxShape::rayIntersect(const Vector3& origin, const Vector3& direction, float maxDistance, float& t,
    Vector3& normal) const
{
    Vector3 boxMin, boxMax;
    getAABB(position, boxMin, boxMax); 

    float tMin = (boxMin.v.x - origin.v.x) / direction.v.x;
    float tMax = (boxMax.v.x - origin.v.x) / direction.v.x;

    if (tMin > tMax) std::swap(tMin, tMax);

    float tyMin = (boxMin.v.y - origin.v.y) / direction.v.y;
    float tyMax = (boxMax.v.y - origin.v.y) / direction.v.y;

    if (tyMin > tyMax) std::swap(tyMin, tyMax);

    if ((tMin > tyMax) || (tyMin > tMax))
        return false;

    if (tyMin > tMin)
        tMin = tyMin;

    if (tyMax < tMax)
        tMax = tyMax;

    float tzMin = (boxMin.v.z - origin.v.z) / direction.v.z;
    float tzMax = (boxMax.v.z - origin.v.z) / direction.v.z;

    if (tzMin > tzMax) std::swap(tzMin, tzMax);

    if ((tMin > tzMax) || (tzMin > tMax))
        return false;

    if (tzMin > tMin)
        tMin = tzMin;

    if (tzMax < tMax)
        tMax = tzMax;

    if (tMin < 0 || tMin > maxDistance)
        return false;

    t = tMin;

    // 计算相交法线
    if (t == tMin) {
        if (tMin == tzMin) normal = Vector3(0, 0, direction.v.z < 0 ? 1 : -1);
        if (tMin == tyMin) normal = Vector3(0, direction.v.y < 0 ? 1 : -1, 0);
        if (tMin == tzMin) normal = Vector3(direction.v.x < 0 ? 1 : -1, 0, 0);
    }

    return true;
}

rapidxml::xml_node<>* BoxShape::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "BoxShape");
    father->append_node(mXmlNode);

    mSize.serialize(doc, mXmlNode, "Size");
    return mXmlNode;
}

void BoxShape::deSerialize(const rapidxml::xml_node<>* node)
{
    auto currentNode = node->first_node("Vector3");
    mSize.deSerialize(currentNode);
}



