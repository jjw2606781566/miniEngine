#pragma once
#include "Engine/Component/Component.h"
#include "Engine/Component/Transform.h"

class AudioListener : public Component
{
public:
    static AudioListener* listener;
    AudioListener();
    ~AudioListener();

    void start() override;
    void update() override;

    Vector3 getPosition();

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value);
    void deSerialize(const rapidxml::xml_node<>* node);
private:
    Vector3 position;
};
