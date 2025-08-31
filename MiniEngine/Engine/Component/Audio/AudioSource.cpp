#include "AudioSource.h"

#include "Engine/AudioSystem/AudioInterface.h"
#include "Engine/Component/GameObject.h"
#include "Engine/Component/Transform.h"

REGISTER_COMPONENT(AudioSource,"AudioSource")

AudioSource::AudioSource()
{
}

AudioSource::~AudioSource()
{
    for (auto& pair : audioClips) {
        stop(pair.first);
    }
    for (auto&  clip :audioClips)
    {
        delete clip.second;
        clip.second = nullptr;
    }
}

void AudioSource::start()
{
    if (getGameObject())
    {
        position = getGameObject()->getTransform() ->getWorldPosition();
    }
    else
    {
        position = Vector3(0, 0, 0);
    }
}

void AudioSource::update()
{
    position = getGameObject()->getTransform()->getWorldPosition();

    if (AudioListener::listener)
    {
        Vector3 listenerPosition = AudioListener::listener->getPosition();

        float distance = (position - listenerPosition).Length();

        for (auto& pair : audioClips)
        {
            // name = pair.first;
            // clip = pair.second; 
            if (mode3DStatus[pair.first])
            {
                float volume = volumeStatus[pair.first];
                float newVolume = calculateVolume(volume, distance);
                pair.second->SetVolume(newVolume);
            }
        }
    }
}

void AudioSource::addAuidioClip(const std::string& name, AudioClip* clip)
{
    if (clip) {
        audioClips[name] = clip;
        mode3DStatus[name] = false;
        volumeStatus[name] = 1.0f;
    }
}

void AudioSource::removeAuidioClip(const std::string& name)
{
    audioClips.erase(name);
    mode3DStatus.erase(name);
    volumeStatus.erase(name);
}

AudioClip* AudioSource::getAuidioClip(const std::string& name)
{
    auto it = audioClips.find(name);
    if (it != audioClips.end()) {
        return it->second;
    }
    return nullptr;
}

bool AudioSource::haveAudioClip(const std::string& name)
{
    return audioClips.find(name) != audioClips.end();
}

void AudioSource::play(const std::string& name)
{
    auto clip = getAuidioClip(name);
    if (clip) 
    {
        if (clip->isPlaying())
            return;
        clip->Play(AudioInterface::sGetInstance());
    }
}

void AudioSource::playLoop(const std::string& name)
{
    auto clip = getAuidioClip(name);
    if (clip && !clip->isPlaying())
    {
        clip->PlayLoop(AudioInterface::sGetInstance());
    }
}

void AudioSource::stop(const std::string& name)
{
    auto clip = getAuidioClip(name);
    if (clip)
    {
        clip->Stop(AudioInterface::sGetInstance());
    }
}

bool AudioSource::isPlay(const std::string& name) const
{
    return audioClips.at(name)->isPlaying();
}


void AudioSource::setVolume(const std::string& name, float v)
{
    volumeStatus[name] = v;
}

float AudioSource::getVolume(const std::string& name) const
{
    auto it = volumeStatus.find(name);
    if (it != volumeStatus.end())
    {
        return it->second;
    }
    return 0.0f;
}

Vector3 AudioSource::getPosition() const
{
    return position;
}

void AudioSource::set3DMode(const std::string& name, bool d)
{
    mode3DStatus[name] = d;
}

bool AudioSource::get3DMode(const std::string& name) const
{
    auto it = mode3DStatus.find(name);
    if (it != mode3DStatus.end())
    {
        return it->second;
    }
    return false;
}

rapidxml::xml_node<>* AudioSource::serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father,
    const TpString& value)
{
    mXmlNode = doc->allocate_node(rapidxml::node_element, "Component");
    father->append_node(mXmlNode);

    mXmlNode->append_attribute(doc->allocate_attribute("name", doc->allocate_string("AudioSource")));

    return mXmlNode;
}

void AudioSource::deSerialize(const rapidxml::xml_node<>* node)
{
    
}

float AudioSource::calculateVolume(float volume, float distance)
{
    if (distance <= minDistance)
    {
        return volume;
    }
    else if (distance >= maxDistance)
    {
        return 0.0f;
    }
    else
    {
        float attenuation = minDistance / distance;
        attenuation *= attenuation;
        return volume * std::min(attenuation, 1.0f);
    }
}

