#pragma once
#include "Engine/Dependencies/rapidxml/rapidxml.hpp"
#include "Engine/Utility/MacroUtility.h"
#include "Engine/Memory/TankinMemory.h"

// an interface
class ISerializable
{
public:
    ISerializable() = default;
    virtual rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value = "") = 0;
    virtual void deSerialize(const rapidxml::xml_node<>* node) = 0;
    rapidxml::xml_node<>* mXmlNode = nullptr; 
};
