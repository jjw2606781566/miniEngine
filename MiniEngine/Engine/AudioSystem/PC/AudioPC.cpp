#include "Engine/AudioSystem/PC/AudioPC.h"

#ifdef WIN32
#include <assert.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <wrl/client.h>
#include <xaudio2.h>
#include <xapofx.h>

bool AudioPC::Init()
{
	//
	// Initialize XAudio2
	//
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		assert(false);
		return false;
	}

	UINT32 flags = 0;
	hr = XAudio2Create(XAudio2Device.GetAddressOf(), flags);
	if (FAILED(hr))
	{
		CoUninitialize();
		assert(false);
		return false;
	}

	//
	// Create a mastering voice
	//
	IXAudio2MasteringVoice* pMasteringVoice = nullptr;
	if (FAILED(hr = XAudio2Device->CreateMasteringVoice(&pMasteringVoice)))
	{
		XAudio2Device.Reset();
		CoUninitialize();
		assert(false);
		return false;
	}

	return true;
}

bool AudioPC::Terminate()
{
	XAudio2Device.Reset();
	CoUninitialize();
	return true;
}

AudioResourceWave* AudioPC::CreatAudioResource(AudioData* data)
{
	AudioResourceWavePC* resource = new AudioResourceWavePC();
	resource->data = data;
	return resource;
}

bool AudioResourceWavePC::isPlaying()
{
	if (SourceVoice)
	{
		XAUDIO2_VOICE_STATE state = {};
		SourceVoice->GetState(&state);
		return state.BuffersQueued > 0;
	}
	return false;
}

bool AudioResourceWavePC::Play(AudioInterface* ad)
{
	HRESULT hr;
	if (SourceVoice)
	{
		SourceVoice->Stop();
		SourceVoice->FlushSourceBuffers();
	}
	else
	{
		AudioPC* adpc = (AudioPC*)ad;

		static_assert(sizeof(WAVEFORMATEXTENSIBLE) >= sizeof(WaveFormatDef), "the layout of WAVEFORMATEXTENSIBLE has changed ?");
		WAVEFORMATEXTENSIBLE fmtExt;
		::ZeroMemory(&fmtExt, sizeof(WAVEFORMATEXTENSIBLE));
		memcpy(&fmtExt, &data->WaveFormat, sizeof(WaveFormatDef));
		
		IXAudio2VoiceCallback* callback = this;
		UINT32 flags = 0;
		if (FAILED(hr = adpc->XAudio2Device->CreateSourceVoice(&SourceVoice, (WAVEFORMATEX*)&fmtExt, flags, XAUDIO2_DEFAULT_FREQ_RATIO, callback)))
		{
			assert(false);
			return false;
		}
	}

	XAUDIO2_BUFFER buffer = {};

	buffer.AudioBytes = data->WaveData->DataBytes;  //size of the audio buffer in bytes
	buffer.pAudioData = (BYTE*)data->WaveData->Data;  //buffer containing audio data
	buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	if (FAILED(hr = SourceVoice->SubmitSourceBuffer(&buffer)))
	{
		assert(false);
		return false;
	}
	
	if (FAILED(hr = SourceVoice->Start(0)))
	{
		assert(false);
		return false;
	}
	
	return true;
}

bool AudioResourceWavePC::PlayLoop(AudioInterface* ad)
{
	HRESULT hr;
	if (SourceVoice)
	{
		SourceVoice->Stop();
		SourceVoice->FlushSourceBuffers();
	}
	else
	{
		AudioPC* adpc = (AudioPC*)ad;

		static_assert(sizeof(WAVEFORMATEXTENSIBLE) >= sizeof(WaveFormatDef), "the layout of WAVEFORMATEXTENSIBLE has changed ?");
		WAVEFORMATEXTENSIBLE fmtExt;
		::ZeroMemory(&fmtExt, sizeof(WAVEFORMATEXTENSIBLE));
		memcpy(&fmtExt, &data->WaveFormat, sizeof(WaveFormatDef));

		IXAudio2VoiceCallback* callback = this;
		UINT32 flags = 0;
		if (FAILED(hr = adpc->XAudio2Device->CreateSourceVoice(&SourceVoice, (WAVEFORMATEX*)&fmtExt, flags, XAUDIO2_DEFAULT_FREQ_RATIO, callback)))
		{
			assert(false);
			return false;
		}
	}

	XAUDIO2_BUFFER buffer = {};

	buffer.AudioBytes = data->WaveData->DataBytes;
	buffer.pAudioData = (BYTE*)data->WaveData->Data;
	
	buffer.LoopBegin = 0; 
	buffer.LoopLength = 0; 
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE; 
	buffer.Flags = 0; 

	if (FAILED(hr = SourceVoice->SubmitSourceBuffer(&buffer)))
	{
		assert(false);
		return false;
	}

	if (FAILED(hr = SourceVoice->Start(0)))
	{
		assert(false);
		return false;
	}
	return true;
}

void AudioResourceWavePC::Stop(AudioInterface* ad)
{
	(void)ad;
	if (SourceVoice)
	{
		SourceVoice->Stop();
		SourceVoice->FlushSourceBuffers();
	}
}

bool AudioResourceWavePC::SetVolume(float volume)
{
	if (SourceVoice)
	{
		HRESULT hr = SourceVoice->SetVolume(volume);
		return SUCCEEDED(hr);
	}
	return false;
}

void AudioResourceWavePC::OnStreamEnd()
{
	
}

void AudioResourceWavePC::OnVoiceError(void* pBufferContext, HRESULT Error)
{
	(void)pBufferContext;
	(void)Error;
	assert(false);
}

AudioResourceWavePC::~AudioResourceWavePC()
{
	if (SourceVoice)
	{
		// https://gamedev.net/forums/topic/606132-is-deleting-memory-from-ixaudio2sourcevoice-possible/4834334/
		SourceVoice->DestroyVoice();
		//SourceVoice = nullptr;
	}
}

#endif