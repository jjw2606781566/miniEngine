#include "Button.h"

REGISTER_COMPONENT(Button, "Button")

Button* Button::sCurrentSelectedButton = nullptr;

void Button::awake()
{
    ImageTGUI::awake();
}

rapidxml::xml_node<>* Button::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    ImageTGUI::serialize(doc, father, value);
    mXmlNode->first_attribute("name")->value(doc->allocate_string("Button"));
    return mXmlNode;
}

void Button::deSerialize(const rapidxml::xml_node<>* node)
{
    ImageTGUI::deSerialize(node);
}

Button::~Button()
{
    if (sCurrentSelectedButton == this)
        sCurrentSelectedButton = nullptr;
}
