#ifndef Audio.h
#define Audio.h

#include <iostream>
#include <AL/al.h>                     //AL libary wont load :)
#include <AL/alc.h>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
include "vector3d.h"



class audio {
	struct data {
		unsigned int sourceid;
		unsigned int bufferid;
		char* buffer;
		data(unsigned int si, unsigned int bi, char* buff) : sourceid(si), bufferid(bi), buffer(buff) {}
	};
	std::vector<data> datas;
	ALCcontext* context; //These both are from AL libary that will set the audio device 
	ALCdevice* device;
	bool bigEndian;
	char* loadWAV(const char* filename, int& ochannels, int& osampleRate, int& osamplesPerSecond, int& osize);
	int convertToInt(const char* data, int len);
	bool isBigEndian();


public:
	audio();
	~audio();
	unsigned int loadAudio(const char* filename, float minGain = 0, float maxGain = 1.0);
	void deleteAudio(unsigned int id);
	void playAudio(unsigned int id);
	void playAudio(unsigned int id, const vector3d& position);           //Vector3d for to get the position? 
	void playAudio(unsigned int id, const vector3d& position, const vector3d& velocity);
	void playAudio(unsigned int id, const vector3d& position, const vector3d& velocity, float gain, bool loop);
	void stopAudio(unsigned int id);
	void setListener(const vector3d& position, const vector3d& orientation); //Orientation meaning where are we looking

};

#endif

