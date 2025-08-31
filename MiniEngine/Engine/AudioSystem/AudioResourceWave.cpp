#include <assert.h>
#include "Engine/AudioSystem/AudioResourceWave.h"



//E WAV File definitions
#define WAVE_FILE_CHUNK_ID_LENGTH				(0x04)
#define WAVE_FILE_CHUNK_SIZE_LENGTH				(0x04)

static bool FindChunkInfo(std::istream& wavFile, const char* strChunkId, int* puChunkSize)
{
	const int chunkIdLen = 4;
	char buffer[chunkIdLen];
	bool found = false;
 
	// 重置读取位置
	wavFile.clear();
	wavFile.seekg(0, std::ios::beg);
 
	while (wavFile.read(buffer, chunkIdLen))
	{
		if (strncmp(buffer, strChunkId, chunkIdLen) == 0)
		{
			found = true;
			break;
		}
		// 如果没找到，回退3个字节，向后移动1字节进行重叠搜索
		wavFile.seekg(-(chunkIdLen - 1), std::ios::cur);
	}
 
	if (!found)
		return false;
 
	// 读取块大小
	int chunkSize = 0;
	if (!wavFile.read(reinterpret_cast<char*>(&chunkSize), sizeof(chunkSize)))
		return false;
 
	*puChunkSize = chunkSize;
	return true;
 
}

bool AudioData::ReadFile(std::istream& wavFile)
{
	wavFile.seekg(0);
	int chunkSize = 0;
	if (!FindChunkInfo(wavFile, "fmt ", &chunkSize))
	{
		assert(false);
		return false;
	}
	memset(&WaveFormat, 0, sizeof(WaveFormatDef));
	wavFile.read((char*)&WaveFormat, chunkSize);

	int dataBytes = 0;
	if (!FindChunkInfo(wavFile, "data", &dataBytes))
	{
		assert(false);
		return false;
	}
		
	WaveData = std::make_shared<WaveDataBuffer>(dataBytes);

	wavFile.read(WaveData->Data, dataBytes);

	return true;
}

bool AudioResourceWave::Play(AudioInterface* ad)
{
	(void)ad;
	assert(false);	// use AudioResourceWavePC or AudioResourceWavePS4
	return false;
}

bool AudioResourceWave::PlayLoop(AudioInterface* ad)
{
	(void)ad;
	assert(false);
	return false;
}


void AudioResourceWave::Stop(AudioInterface* ad)
{
	(void)ad;
	assert(false);	// use AudioResourceWavePC or AudioResourceWavePS4
}

bool AudioResourceWave::SetVolume(float volume)
{
	(void)volume;
	assert(false);
	return false;
}

bool AudioResourceWave::isPlaying()
{
	assert(false);
	return false;
}
