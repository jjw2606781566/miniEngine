#pragma once
#include "Engine/math/math.h"
#include "Engine/Scene/ISerializable.h"

enum class ShapeType {
    Box,
    Sphere
};

class BaseShape : public ISerializable{
public:
    virtual ~BaseShape();

    virtual ShapeType getType() const = 0;
    virtual bool isConvex() const = 0;
    virtual void getAABB(const Vector3& position, Vector3& min, Vector3& max) const = 0;
    virtual bool localIsInside(const Vector3& point) const = 0;
    virtual void project(const Vector3& position, const Vector3& axis, float& minProj,
                         float& maxProj, Vector3& minPoint, Vector3& maxPoint) const = 0;
    void setPosition(const Vector3& pos) { position = pos; }
    
    float getVolume() const { return volume; }
    Vector3 getPosition() const { return position; }
    virtual bool checkCollision(const BaseShape& other) const = 0;
    virtual bool rayIntersect(const Vector3& origin, const Vector3& direction, float maxDistance, float& t, Vector3& normal) const = 0;

    virtual rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value) = 0;
    virtual void deSerialize(const rapidxml::xml_node<>* node) = 0;
protected:
    Vector3 position;
    float volume = 0;        

};
