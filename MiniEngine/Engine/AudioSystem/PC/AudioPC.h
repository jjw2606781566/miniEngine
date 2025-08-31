#pragma once
#ifdef WIN32

#include "Engine/AudioSystem/AudioInterface.h"
#include "Engine/AudioSystem/AudioResourceWave.h"
#include <wrl/client.h>
#include <xaudio2.h>

class AudioPC: public AudioInterface
{
	friend class AudioResourceWavePC;
public:
	virtual bool Init() override;
	virtual bool Terminate() override;
	virtual AudioResourceWave* CreatAudioResource(AudioData* data) override;
protected:
	Microsoft::WRL::ComPtr<IXAudio2> XAudio2Device;
};


class AudioResourceWavePC : public AudioResourceWave, public IXAudio2VoiceCallback
{
	friend class AudioPC;
public:
	virtual bool Play(AudioInterface* ad) override;
	virtual bool PlayLoop(AudioInterface* ad) override;
	virtual void Stop(AudioInterface* ad) override;
	virtual bool SetVolume(float volume) override;
	virtual bool isPlaying() override;
	
	virtual ~AudioResourceWavePC() override;

	// IXAudio2VoiceCallback
	virtual void OnVoiceProcessingPassStart(UINT32 bytesRequired) { (void)bytesRequired; }
	virtual void OnVoiceProcessingPassEnd() {}
	virtual void OnStreamEnd();
	virtual void OnBufferStart(void* pBufferContext) { (void)pBufferContext; }
	virtual void OnBufferEnd(void* pBufferContext) { (void)pBufferContext; }
	virtual void OnLoopEnd(void* pBufferContext) { (void)pBufferContext; }
	virtual void OnVoiceError(void* pBufferContext, HRESULT Error);

protected:
	IXAudio2SourceVoice* SourceVoice = nullptr;
};

#endif
