#pragma once

#include "BaseShape.h"


class BoxShape : public BaseShape {
public:
    explicit BoxShape(const Vector3& size);

    ShapeType getType() const override;
    bool isConvex() const override;
    void getAABB(const Vector3& position, Vector3& min, Vector3& max) const override;
    bool localIsInside(const Vector3& point) const override;
    void project(const Vector3& position, const Vector3& axis, float& minProj,
                 float& maxProj, Vector3& minPoint, Vector3& maxPoint) const override;

    Vector3 getSize() const;
    void setSize(const Vector3& newSize) { mSize = newSize;}
    bool checkCollision(const BaseShape& other) const override;
    bool rayIntersect(const Vector3& origin, const Vector3& direction, float maxDistance, float& t, Vector3& normal) const override;

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;
private:
    Vector3 mSize;
    /**
     * 定义了float的极限值
     */
    const float largeFloat = 1e30f;  
    const float smallFloat = -1e30f;
};