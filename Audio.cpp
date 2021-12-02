#include "Audio.h"


//WAV files only taking audio playing system


char* audio::loadWAV(const char* filename, int& ochannels, int& osampleRate, int& osamplesPerSecond, int& osize)
{
	std::ifstream in(filename, std::ios::binary);
	char tmpBuffer[4];
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "RIFF", 4) != 0)
	{
		std::cout << "This is not a valid WAV file (1) " << tmpBuffer[0] << tmpBuffer[1] << tmpBuffer[2] << tmpBuffer[3] << tmpBuffer[4] << '\n';
		return 0;
	}
	in.read(tmpBuffer, 4);
	int sizeInBytes = convertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "WAVE", 4) != 0)
	{
		std::cout << "This is not a valid WAV file (2) " << tmpBuffer[0] << tmpBuffer[1] << tmpBuffer[2] << tmpBuffer[3] << tmpBuffer[4] << '\n';
		return 0;
	}
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "fmt ", 4) != 0)
	{
		std::cout << "This is not a valid WAV file (3) " << tmpBuffer[0] << tmpBuffer[1] << tmpBuffer[2] << tmpBuffer[3] << tmpBuffer[4] << '\n';
		return 0;
	}
	in.read(tmpBuffer, 4);
	int fmtSize = convertToInt(tmpBuffer, 4);
	if (fmtSize != 16)
	{
		std::cout << "Sorry only PCM (not compressed WAV)" << fmtSize << '\n';
		return 0;
	}
	in.read(tmpBuffer, 2);
	int PCM = convertToInt(tmpBuffer, 2);
	if (PCM != 1)
	{
		std::cout << "Sorry only PCM (not compressed WAV)\n";
		return 0;
	}
	in.read(tmpBuffer, 2);
	int channels = convertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 4);
	int samples = convertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 4);                                              //<--- Al studion default stereo buffer system
	int byteRate = convertToInt(tmpBuffer, 4);
	in.read(tmpBuffer, 2);
	int blockAlign = convertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 2);
	int bitsPerSample = convertToInt(tmpBuffer, 2);
	in.read(tmpBuffer, 4);
	if (strncmp(tmpBuffer, "data", 4) != 0)
	{
		std::cout << "This is not a valid WAV file (4) " << tmpBuffer[0] << tmpBuffer[1] << tmpBuffer[2] << tmpBuffer[3] << '\n';
		//	return 0;
	}
	in.read(tmpBuffer, 4);
	int dataSize = convertToInt(tmpBuffer, 4);
	if (dataSize <= 0)
	{
		std::cout << "This is not a valid WAV file (4) " << dataSize << '\n';
		return 0;
	}
	char* data = new char[dataSize];
	in.read(data, dataSize);
	std::cout << sizeInBytes << ' ' << fmtSize << ' ' << channels << ' ' << samples << ' ' << byteRate << ' ' << blockAlign << ' ' << bitsPerSample << ' ' << dataSize << std::endl;
	ochannels = channels;
	osampleRate = bitsPerSample;
	osamplesPerSecond = samples;
	osize = dataSize;
	return data;
}

int audio::convertToInt(const char* data, int len)
{
	int a = 0;
	if (!bigEndian)
		for (int i = 0; i < len; i++)
			((char*)&a)[i] = data[i];
	else
		for (int i = 0; i < len; i++)
			((char*)&a)[sizeof(int) - i - 1] = data[i];
	return a;
}

bool audio::isBigEndian()
{
	int a = 1;
	return (((unsigned char*)&a)[0] == 0);
}

audio::audio()
{
	bigEndian = isBigEndian();
	device = alcOpenDevice(NULL);
	if (device == NULL)
	{
		std::cout << "couldn't open device" << std::endl;
		context = NULL;
		return;
	}
	context = alcCreateContext(device, NULL);
	if (context == NULL)
	{
		std::cout << "couldn't open context" << std::endl;
		return;
	}
	alcMakeContextCurrent(context);
}

audio::~audio()
{
	for (int i = 0; i < datas.size(); i++)
	{
		alDeleteSources(1, &datas[i].sourceid);
		alDeleteBuffers(1, &datas[i].bufferid);        //Frees the loaded files and buffers under use

		delete[] datas[i].buffer;
	}
	alcDestroyContext(context);
	alcCloseDevice(device);
}

unsigned int audio::loadAudio(const char* filename, float minGain, float maxGain)
{
	unsigned int sourceid;
	int channel, sampleRate, samplesPerSecond, size;
	alGenSources(1, &sourceid);
	char* bufferdata = loadWAV(filename, channel, sampleRate, samplesPerSecond, size);
	unsigned int bufferid;
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
	datas.push_back(data(sourceid, bufferid, bufferdata));
	return sourceid;
}

void audio::deleteAudio(unsigned int id)
{
	for (int i = 0; i < datas.size(); i++)
		if (datas[i].sourceid == id)        //might be wrong not sure since cant test it out 
		{
			alSourcei(datas[i].sourceid, AL_BUFFER, NULL);
			alDeleteBuffers(1, &datas[i].bufferid);
			delete[] datas[i].buffer;
			alDeleteSources(1, &datas[i].sourceid);
			datas.erase(datas.begin() + i);
			return;
		}
}

void audio::playAudio(unsigned int id)
{
	alSourcePlay(id);
}

void audio::playAudio(unsigned int id, const vector3d& position)
{
	alSource3f(id, AL_POSITION, position.x, position.y, position.z);
	alSourcePlay(id);

}

void audio::playAudio(unsigned int id, const vector3d& position, const vector3d& velocity)
{
	alSource3f(id, AL_POSITION, position.x, position.y, position.z);
	alSource3f(id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	alSourcePlay(id);
}

void audio::playAudio(unsigned int id, const vector3d& position, const vector3d& velocity, float gain, bool loop)
{
	alSource3f(id, AL_POSITION, position.x, position.y, position.z);
	alSource3f(id, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
	alSourcef(id, AL_GAIN, gain);
	alSourcePlay(id);
}

void audio::stopAudio(unsigned int id)
{
	alSourceStop(id); //this stops the audio when it should (i know, dont worry guys i got you)
}

void audio::setListener(const vector3d& position, const vector3d& orientation)
{
	alListener3f(AL_POSITION, position.x, position.y, position.z);
	float f[] = { orientation.x,orientation.y,orientation.z,0,1,0 };
	alListenerfv(AL_ORIENTATION, f);
}







