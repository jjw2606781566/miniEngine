#pragma once
#include "Engine/Component/Component.h"
#include "Engine/AudioSystem/AudioClip.h"
#include "Engine/math/math.h"
#include <memory>

#include "AudioListener.h"


class AudioSource : public Component
{
public:
    AudioSource();
    ~AudioSource() override ;

    // 初始化音频源
    void start() override;
    void update() override;
    void addAuidioClip(const std::string& name, AudioClip* clip);
    void removeAuidioClip(const std::string& name);
    AudioClip* getAuidioClip(const std::string& name);
    bool haveAudioClip(const std::string& name);
    // 播放控制
    void play(const std::string& name);
    void playLoop(const std::string& name);
    void stop(const std::string& name);
    bool isPlay(const std::string& name) const;
    
    void setVolume(const std::string& name,float v);
    float getVolume(const std::string& name) const;
    Vector3 getPosition() const;
    void set3DMode(const std::string& name, bool d);
    bool get3DMode(const std::string& name) const;
    
    friend class Audiolistener;

    rapidxml::xml_node<>* serialize(rapidxml::xml_document<>* doc, rapidxml::xml_node<>* father, const TpString& value);
    void deSerialize(const rapidxml::xml_node<>* node);

private:
    std::map<std::string, AudioClip*> audioClips;
    std::map<std::string, bool> mode3DStatus;
    std::map<std::string, float> volumeStatus;

    const float minDistance = 30.0f;
    const float maxDistance = 150.0f;

    float calculateVolume(float volume, float distance);
    
    Vector3 position;
};
