#pragma once
#include "Engine/Component/Component.h"
#include <utility>

#include "Engine/Component/Physics/RigidBody.h"
#include "Engine/math/PC/Vector3PC.h"

class Obstacle : public Component
{
public:
    
    void start() override;
    void onDestory() override;
    void update() override;

    void projectToGrid();

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value);
    void deSerialize(const rapidxml::xml_node<>* node);
private:
    RigidBody* mRigidBody = nullptr;
    std::vector<std::pair<int, int>> markedTiles;
    void markBox(const Vector3& position, const Vector3& size);
    void markSphere(const Vector3& position, float radius);
    void clearGridMarks();
    static bool isValidGridPosition(int x, int z);
};
