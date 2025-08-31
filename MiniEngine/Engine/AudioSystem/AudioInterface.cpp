#include "AudioInterface.h"

#include "PC/AudioPC.h"
#include "PS4/AudioPS4.h"

AudioInterface* AudioInterface::sInstance = nullptr;

void AudioInterface::sInit()
{
#ifdef WIN32
    sInstance = new AudioPC();
    sInstance->Init();
#endif
#ifdef ORBIS
    sInstance = new AudioPS4();
    sInstance->Init();
#endif
}