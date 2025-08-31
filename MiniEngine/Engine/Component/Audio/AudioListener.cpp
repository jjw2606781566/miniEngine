#include "AudioListener.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"

REGISTER_COMPONENT(AudioListener,"AudioListener")

AudioListener* AudioListener::listener = nullptr;

AudioListener::AudioListener()
{
    if (listener != nullptr)
    {
        DEBUG_PRINT("audioListener already have");
    }
    listener = this;
}

AudioListener::~AudioListener()
{
    if (listener == this) {
        listener = nullptr;
    }
}

void AudioListener::start()
{
    if (getGameObject())
    {
        position = getGameObject()->getTransform()->getWorldPosition();
    }
    else
    {
        position = kZeroVector3;
    }
}

void AudioListener::update()
{
    if (getGameObject())
    {
        position = getGameObject()->getTransform()->getWorldPosition();
    }
    else
    {
        position = kZeroVector3;
    }
}

Vector3 AudioListener::getPosition()
{
    return position;
}

rapidxml::xml_node<>* AudioListener::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("AudioListener")));

    return mXmlNode;
}

void AudioListener::deSerialize(const rapidxml::xml_node<>* node)
{
    
}
