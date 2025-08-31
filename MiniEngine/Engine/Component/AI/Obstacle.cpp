#include "Obstacle.h"

#include "Engine/AISystem/MapData.h"
#include "Engine/AISystem/NavigationMap.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"
#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/Physical/Shape/BoxShape.h"
#include "Engine/Physical/Shape/SphereShape.h"

REGISTER_COMPONENT(Obstacle, "Obstacle")

void Obstacle::start()
{
    mRigidBody = dynamic_cast<RigidBody*>(getGameObject()->getComponent("RigidBody"));
    projectToGrid();
}

void Obstacle::onDestory()
{
    clearGridMarks();
    mRigidBody = nullptr;
}

void Obstacle::update()
{
    // mass为0就是静止, 不用计算
    if (mRigidBody && mRigidBody->getMass()!=0)
    {
        projectToGrid();
    }
    
}

void Obstacle::projectToGrid()
{
    clearGridMarks();
    if (mRigidBody->getShape<BaseShape>() == nullptr)
        return;
    ShapeType shapeType = mRigidBody->getShape<BaseShape>()->getType();
    Vector3 position = mRigidBody->getPosition();
    if (shapeType == ShapeType::Box)
    {
        Vector3 size = mRigidBody->getShape<BoxShape>()->getSize();
        markBox(position, size);
    }
    else if (shapeType == ShapeType::Sphere)
    {
        float radius = mRigidBody->getShape<SphereShape>()->getRadius();
        markSphere(position, radius);
    }
    else
    {
        DEBUG_PRINT("RigidBody has not ShapeType");
    }
}

rapidxml::xml_node<>* Obstacle::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("Obstacle")));
    return mXmlNode;
}

void Obstacle::deSerialize(const rapidxml::xml_node<>* node)
{
    
}

void Obstacle::markBox(const Vector3& position, const Vector3& size)
{
    // 将世界坐标转换为网格坐标进行标记
    int minX = static_cast<int>((position.v.x - 0.5 * size.v.x - 2 - 1) / 2);
    int maxX = static_cast<int>((position.v.x + 0.5 * size.v.x + 2 - 1) / 2);
    int minZ = static_cast<int>((position.v.z - 0.5 * size.v.z - 2 - 1) / 2);
    int maxZ = static_cast<int>((position.v.z + 0.5 * size.v.z + 2 - 1) / 2);

    for (int z = minZ; z <= maxZ; ++z) {
        for (int x = minX; x <= maxX; ++x) {
            if (isValidGridPosition(x, z)) {
                // 标记为障碍
                MapData::getInstance().setTile(x, z, 1); 
                markedTiles.emplace_back(x, z);
            }
        }
    }
}

void Obstacle::markSphere(const Vector3& position, float radius)
{
    int minX = static_cast<int>((position.v.x - radius - 2 - 1) / 2);
    int maxX = static_cast<int>((position.v.x + radius + 2 - 1) / 2);
    int minZ = static_cast<int>((position.v.z - radius - 2 - 1) / 2);
    int maxZ = static_cast<int>((position.v.z + radius + 2 - 1) / 2);
    
    for (int z = minZ; z <= maxZ; ++z) {
        for (int x = minX; x <= maxX; ++x) {
            float dx = x - position.v.x;
            float dz = z - position.v.z;
            if (dx*dx + dz*dz <= radius*radius) {
                if (isValidGridPosition(x, z)) {
                    // 标记为障碍
                    MapData::getInstance().setTile(x,z,1); 
                    markedTiles.emplace_back(x, z);
                }
            }
        }
    }
}

void Obstacle::clearGridMarks()
{
    for (auto& pos : markedTiles) {
        if (isValidGridPosition(pos.first, pos.second)) {
            // 清除标记
            MapData::getInstance().setTile(pos.first,pos.second,0); 
        }
    }
    markedTiles.clear();
}


bool Obstacle::isValidGridPosition(int x, int z)
{
    return x >= 0 && z >= 0 && 
           x < MapData::getInstance().getMapWidth() && 
           z < MapData::getInstance().getMapHeight();
}
