#pragma once

#include "../Component.h"
#include "Engine/render/MeshData.h"

class RenderMeshResource;
class MeshFilter:public Component
{
    friend class ComponentFactory;
public:
    void start() override;
    void update() override;
    
    void setMesh(const TpString& meshName){mMeshName = meshName;}

    MeshData getMeshData() const;
    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
        const TpString& value) override;
    void deSerialize(const rapidxml::xml_node<>* node) override;

private:
    TpString mMeshName = "Cube";
};
