#include "MeshFilter.h"
#include "../GameObject.h"
#include "Engine/FileManager/FileManager.h"
#include "Engine/render/MeshData.h"

static ComponentRegister::Register<MeshFilter> TankControllerRegister("MeshFilter");

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MeshFilter::start()
{
    DEBUG_PRINT("<%s> MeshFilter Component Start()\n", mGameObject->getName().c_str());
}

void MeshFilter::update()
{
    DEBUG_PRINT("<%s> MeshFilter Component Update()\n", mGameObject->getName().c_str());
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MeshData MeshFilter::getMeshData() const
{
    return FileManager::sGetLoadedBolbFile<MeshData>(mMeshName);
}

rapidxml::xml_node<>* MeshFilter::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("MeshFilter")));

    mXmlNode->append_attribute(doc->allocate_attribute("MeshName", doc->allocate_string(mMeshName.c_str())));
    return mXmlNode;
}

void MeshFilter::deSerialize(const rapidxml::xml_node<>* node)
{
    mMeshName = node->first_attribute("MeshName")->value();
}
