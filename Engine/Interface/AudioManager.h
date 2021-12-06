#pragma once
#include <Core.h>
#include <AL/al.h>
#include <AL/alc.h>

class AudioManager
{
public:
	static void Init();
	/// <summary>
	/// Accepts Signed 16-bit PCM WAV Files.
	/// </summary>
	/// <param name="filename"></param>
	/// <param name="minGain"></param>
	/// <param name="maxGain"></param>
	/// <returns></returns>
	static uint LoadAudio(String filename, float minGain = 0, float maxGain = 1.0);
	static void DeleteAudio(uint id);
	static void PlayAudio(uint id);
	static void PlayAudio(uint id, const Vector &position);
	static void PlayAudio(uint id, const Vector &position, const Vector& velocity);
	static void PlayAudio(uint id, const Vector &position, const Vector& velocity, float gain);

	static void SetAudioPosition(uint id, const Vector& position);
	static void SetAudioVelocity(uint id, const Vector& velocity);
	static void SetAudioGain(uint id, const float gain);
	static void SetAudioRelativity(uint id, const bool relative);

	static void StopAudio(uint id);
	static void SetListener(const Vector &position, const Vector &orientation); //Orientation meaning where are we looking
	static void Destroy();

private:
	struct AudioData {
		uint Sourceid;
		uint Bufferid;
		char* Buffer;
		AudioData(uint si, uint bi, char* buff) : Sourceid(si), Bufferid(bi), Buffer(buff) {}
	};

	AudioManager() {};
	~AudioManager() {};

	static char* LoadWAV(const char* filename, int& ochannels, int& osampleRate, int& osamplesPerSecond, int& osize);
	static int ConvertToInt(const char* data, int len);
	static bool IsBigEndian();

	inline static std::vector<AudioData> AudioDataStorage;
	inline static ALCcontext* Context = nullptr; //These both are from AL libary that will set the audio device 
	inline static ALCdevice* Device = nullptr;
	inline static bool BigEndian = false;
};

