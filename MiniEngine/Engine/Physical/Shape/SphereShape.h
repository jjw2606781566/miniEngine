#pragma once
#include "BaseShape.h"
#include "Engine/math/math.h"
#include "Engine/Scene/ISerializable.h"

class SphereShape : public BaseShape {
public:
    explicit SphereShape(float radius);

    ShapeType getType() const override;
    bool isConvex() const override;
    void getAABB(const Vector3& position, Vector3& min, Vector3& max) const override;
    bool localIsInside(const Vector3& point) const override;
    void project(const Vector3& position, const Vector3& axis, float& minProj,
                 float& maxProj, Vector3& minPoint, Vector3& maxPoint) const override;

    bool checkCollision(const BaseShape& other) const override;
    bool rayIntersect(const Vector3& origin, const Vector3& direction, float maxDistance, float& t, Vector3& normal) const override;
    float getRadius() const;

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
private:
    float radius = 1;
};
