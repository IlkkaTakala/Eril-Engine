#include "AudioManager.h"
#include <fstream>
#include <cstring>
#include <Settings.h>

//Public Functions

void AudioManager::Init()
{
	BigEndian = IsBigEndian();
	Device = alcOpenDevice(NULL);
	if (Device == NULL)
	{
		Console::Error("AudioManger: Couldn't open device");
		Context = NULL;
		return;
	}
	Context = alcCreateContext(Device, NULL);
	if (Context == NULL)
	{
		Console::Error("AudioManger: Couldn't open context");
		return;
	}
	alcMakeContextCurrent(Context);
}

uint AudioManager::LoadAudio(String filename, float minGain, float maxGain)
{
	String folder = INI->GetValue("Engine", "DataFolder") + "/Audio/";
	filename = folder + filename;

	uint sourceid;
	int channel, sampleRate, samplesPerSecond, size;
	alGenSources(1, &sourceid);
	char* bufferdata = LoadWAV(filename.c_str(), channel, sampleRate, samplesPerSecond, size);
	uint bufferid;
	alGenBuffers(1, &bufferid);
	int format;
	if (channel == 1)                  //might not work send help pls
	{
		if (sampleRate == 8)
			format = AL_FORMAT_MONO8;
		else
			format = AL_FORMAT_MONO16;
	}
	else {
		if (sampleRate == 8)
			format = AL_FORMAT_STEREO8;
		else
			format = AL_FORMAT_STEREO16;
	}
	alBufferData(bufferid, format, bufferdata, size, samplesPerSecond);
	alSourcei(sourceid, AL_BUFFER, bufferid);
	alSourcef(sourceid, AL_MIN_GAIN, minGain);
	alSourcef(sourceid, AL_MAX_GAIN, maxGain);
	alSourcei(sourceid, AL_SOURCE_RELATIVE, AL_TRUE);
	AudioDataStorage.push_back(AudioData(sourceid, bufferid, bufferdata));

	alSourcef(sourceid, AL_PITCH, 1);
	alSourcef(sourceid, AL_GAIN, 1.0f);
	alSource3f(sourceid, AL_POSITION, 0, 0, 0);
	alSource3f(sourceid, AL_VELOCITY, 0, 0, 0);
	alSourcei(sourceid, AL_LOOPING, AL_FALSE);
	return sourceid;
}

void AudioManager::DeleteAudio(uint id)
{
	for (int i = 0; i < AudioDataStorage.size(); i++)
		if (AudioDataStorage[i].Sourceid == id)        //might be wrong not sure since cant test it out 
		{
			alSourcei(AudioDataStorage[i].Sourceid, AL_BUFFER, NULL);
			alDeleteBuffers(1, &AudioDataStorage[i].Bufferid);
			delete[] AudioDataStorage[i].Buffer;
			alDeleteSources(1, &AudioDataStorage[i].Sourceid);
			AudioDataStorage.erase(AudioDataStorage.begin() + i);
			return;
		}
}

void AudioManager::PlayAudio(uint id)
{
	alSourcePlay(id);
}

void AudioManager::PlayAudio(uint id, const Vector& position)
{
	alSource3f(id, AL_POSITION, position.X, position.Z, position.Y);
	alSourcePlay(id);
}

void AudioManager::PlayAudio(uint id, const Vector& position, const Vector& velocity)
{
	alSource3f(id, AL_POSITION, position.X, position.Z, position.Y);
	alSource3f(id, AL_VELOCITY, velocity.X, velocity.Z, velocity.Y);
	alSourcePlay(id);
}

void AudioManager::PlayAudio(uint id, const Vector& position, const Vector& velocity, float gain)
{
	alSource3f(id, AL_POSITION, position.X, position.Z, position.Y);
	alSource3f(id, AL_VELOCITY, velocity.X, velocity.Z, velocity.Y);
	alSourcef(id, AL_GAIN, gain);
	alSourcePlay(id);
}

void AudioManager::SetAudioPosition(uint id, const Vector& position)
{
	alSource3f(id, AL_POSITION, position.X, position.Z, position.Y);
}

void AudioManager::SetAudioVelocity(uint id, const Vector& velocity)
{
	alSource3f(id, AL_VELOCITY, velocity.X, velocity.Z, velocity.Y);
}

void AudioManager::SetAudioGain(uint id, const float gain)
{
	alSourcef(id, AL_GAIN, gain);
}

void AudioManager::SetAudioRelativity(uint id, const bool relative)
{
	if (relative)
	{
		alSourcei(id, AL_SOURCE_RELATIVE, AL_TRUE);
	}
	else
	{
		alSourcei(id, AL_SOURCE_RELATIVE, AL_FALSE);
	}
}

void AudioManager::StopAudio(uint id)
{
	alSourceStop(id); //this stops the audio when it should (i know, dont worry guys i got you)
}

void AudioManager::SetListener(const Vector& position, const Vector& orientation, const Vector& up)
{
	alListener3f(AL_POSITION, position.X, position.Z, position.Y);
	float f[] = {orientation.X, -orientation.Z, orientation.Y, up.X, -up.Z, up.Y };
	alListenerfv(AL_ORIENTATION, f);
}

void AudioManager::Destroy()
{
	for (int i = 0; i < AudioDataStorage.size(); i++)
	{
		alDeleteSources(1, &AudioDataStorage[i].Sourceid);
		alDeleteBuffers(1, &AudioDataStorage[i].Bufferid);        //Frees the loaded files and buffers under use

		delete[] AudioDataStorage[i].Buffer;
	}
	alcDestroyContext(Context);
	alcCloseDevice(Device);
}

//Private Functions

char* AudioManager::LoadWAV(const char* filename, int& ochannels, int& osampleRate, int& osamplesPerSecond, int& osize)
{
	std::ifstream in(filename, std::ios::binary);
	if (!in.is_open())
	{
		Console::Warning("AudioManger: Cannot open file " + String(filename));
		return 0;
	}

	char tmpBuffer[4];
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "RIFF", 4) != 0)
	{
		Console::Warning("AudioManger: File is not a valid WAVE file (header doesn't begin with RIFF) File:" + String(filename));
		return 0;
	}
	in.read(tmpBuffer, 4);
	int sizeInBytes = ConvertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "WAVE", 4) != 0)
	{
		Console::Warning("File is not a valid WAVE file (header doesn't contain WAVE) FIle:" + String(filename));
		return 0;
	}
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "fmt ", 4) != 0)
	{
		Console::Warning("AudioManger: Could not read fmt! File:" + String(filename));
		return 0;
	}
	in.read(tmpBuffer, 4);
	int fmtSize = ConvertToInt(tmpBuffer, 4);
	if (fmtSize != 16)
	{
		Console::Warning("AudioManger: Sorry, only PCM (not compressed WAV) File:" + String(filename));
		return 0;
	}
	in.read(tmpBuffer, 2);
	int PCM = ConvertToInt(tmpBuffer, 2);
	if (PCM != 1)
	{
		Console::Warning("AudioManger: Sorry, only PCM (not compressed WAV) File:" + String(filename));
		return 0;
	}
	in.read(tmpBuffer, 2);
	int channels = ConvertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 4);
	int samples = ConvertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 4);                                              //<--- Al studion default stereo buffer system
	int byteRate = ConvertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 2);
	int blockAlign = ConvertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 2);
	int bitsPerSample = ConvertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "data", 4) != 0)
	{
		Console::Warning("AudioManger: This is not a valid WAV file (4) File:" + String(filename));
		//	return 0;
	}
	in.read(tmpBuffer, 4);
	int dataSize = ConvertToInt(tmpBuffer, 4);
	if (dataSize <= 0)
	{
		Console::Warning("AudioManger: This is not a valid WAV file (4) File:" + String(filename));
		return 0;
	}
	char* data = new char[dataSize];
	in.read(data, dataSize);
	ochannels = channels;
	osampleRate = bitsPerSample;
	osamplesPerSecond = samples;
	osize = dataSize;
	return data;
}

int AudioManager::ConvertToInt(const char* data, int len)
{
	int a = 0;
	if (!BigEndian)
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = data[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[sizeof(int) - i - 1] = data[i];
	return a;
}

bool AudioManager::IsBigEndian()
{
	int a = 1;
	return (((unsigned char*)&a)[0] == 0);
}

