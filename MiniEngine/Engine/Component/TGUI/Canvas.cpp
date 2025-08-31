#include "Canvas.h"

#include "Engine/Component/GameObject.h"

REGISTER_COMPONENT(Canvas, "Canvas");
Canvas* Canvas::sCurrentCanvas = nullptr;

void Canvas::awake()
{
    mTransform = mGameObject->getTransform();
}

rapidxml::xml_node<>* Canvas::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
                                        const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("Canvas")));

    mXmlNode->append_attribute(doc->allocate_attribute("Width", doc->allocate_string(std::to_string(mWidth).c_str())));
    mXmlNode->append_attribute(doc->allocate_attribute("Height", doc->allocate_string(std::to_string(mHeight).c_str())));

    return mXmlNode;
}

void Canvas::deSerialize(const rapidxml::xml_node<>* node)
{
    mWidth = std::stoi(node->first_attribute("Width")->value());
    mHeight = std::stoi(node->first_attribute("Height")->value());
}
