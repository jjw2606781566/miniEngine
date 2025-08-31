#pragma once

#include "Engine/AudioSystem/AudioResourceWave.h"

class AudioInterface
{
public:
	static void sInit();
	static AudioInterface* sGetInstance()
	{
		return sInstance;
	}
	virtual AudioResourceWave* CreatAudioResource(AudioData* data) = 0;
	virtual bool Init() { return false; }
	virtual bool Terminate() { return false; }
private:
	static AudioInterface* sInstance;
};