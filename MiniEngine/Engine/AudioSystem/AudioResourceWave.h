#pragma once
#include <iostream>
#include <memory>
class AudioInterface;

// mmreg.h
struct WaveFormatDef
{
	uint16_t    wFormatTag;        /* format type */
	uint16_t    nChannels;         /* number of channels (i.e. mono, stereo...) */
	uint32_t	nSamplesPerSec;    /* sample rate */
	uint32_t	nAvgBytesPerSec;   /* for buffer estimation */
	uint16_t    nBlockAlign;       /* block size of data */
	uint16_t    wBitsPerSample;    /* Number of bits per sample of mono data */
	uint16_t    cbSize;            /* The count in bytes of the size of */
};

struct WaveFormatExtDef : public WaveFormatDef
{
	union
	{
		uint16_t wValidBitsPerSample;       /* bits of precision  */
		uint16_t wSamplesPerBlock;          /* valid if wBitsPerSample==0 */
		uint16_t wReserved;                 /* If neither applies, set to zero. */
	} Samples;
	uint32_t	dwChannelMask;			/* which channels are */
	/* present in stream  */
	uint32_t    SubFormat[4];			// GUID
};

struct WaveDataBuffer
{
	int DataBytes = 0;
	char* Data = nullptr;
	WaveDataBuffer(int _bytes)
		:DataBytes(_bytes)
	{
		Data = (char*)malloc(_bytes);
	}

	~WaveDataBuffer()
	{
		if (Data)
			free(Data);
		DataBytes = 0;
	}
};

//FileManager 存AudioData，获取时创建一个AudioResourceWave返回

struct AudioData
{
	bool ReadFile(std::istream& wavFile);
	WaveFormatDef WaveFormat;

	std::shared_ptr<WaveDataBuffer> WaveData;
};

class AudioResourceWave
{
public:
	virtual bool Play(AudioInterface* ad);
	virtual bool PlayLoop(AudioInterface* ad);
	virtual void Stop(AudioInterface* ad);
	virtual bool SetVolume(float volume);
	virtual bool isPlaying();

	virtual ~AudioResourceWave() = default;
protected:
	AudioData* data = nullptr;
};