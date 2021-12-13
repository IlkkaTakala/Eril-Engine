#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
07.12.2021
*/

#include <Core.h>
#include <ECS/Component.h>

struct AudioComponent : public Component
{
public:
	friend class AudioControllerSystem;

	AudioComponent() 
	{

	}
	AudioComponent(const AudioComponent& c) : Component(static_cast<Component>(c))
	{
		SourceID = c.SourceID;

		bPositionChanged = c.bPositionChanged;
		Position = c.Position;
		Velocity = c.Velocity;

		bSettingChanged = c.bSettingChanged;
		Pitch = c.Pitch;
		Gain = c.Gain;
		bLoop = c.bLoop;
		bSourceRelative = c.bSourceRelative;
		bStartPlaying = c.bStartPlaying;
		bStopPlaying = c.bStopPlaying;
		
	}

	AudioComponent(AudioComponent&& c) noexcept : Component(std::move(c))
	{
		SourceID = std::move(c.SourceID);

		bPositionChanged = std::move(c.bPositionChanged);
		Position = std::move(c.Position);
		Velocity = std::move(c.Velocity);

		bSettingChanged = std::move(c.bSettingChanged);
		Pitch = std::move(c.Pitch);
		Gain = std::move(c.Gain);
		bLoop = std::move(c.bLoop);
		bSourceRelative = std::move(c.bSourceRelative);
		bStartPlaying = std::move(c.bStartPlaying);
		bStopPlaying = std::move(c.bStopPlaying);
	}

	AudioComponent& operator=(const AudioComponent& c)
	{
		SourceID = c.SourceID;

		bPositionChanged = c.bPositionChanged;
		Position = c.Position;
		Velocity = c.Velocity;

		bSettingChanged = c.bSettingChanged;
		Pitch = c.Pitch;
		Gain = c.Gain;
		bLoop = c.bLoop;
		bSourceRelative = c.bSourceRelative;
		bStartPlaying = c.bStartPlaying;
		bStopPlaying = c.bStopPlaying;
		
		return *this;
	}

	void SetSourceID(uint id) { SourceID = id; }
	void SetPosition(Vector pos) { bPositionChanged = true; Position = pos; }
	void SetVelocity(Vector vel) { bPositionChanged = true; Velocity = vel; }

	//Change Sound Settings
	void SetPitch(float pitch) { bSettingChanged = true;  Pitch = pitch; }
	void SetGain(float gain) { bSettingChanged = true;  Gain = gain; }
	void SetLooping(bool loop) { bSettingChanged = true;  bLoop = loop; }
	void SetSourceRelative(bool relative) { bSettingChanged = true;  bSourceRelative = relative; }

	void Play() { bStartPlaying = true; }
	void Stop() { bStopPlaying = true; }

	Vector GetPosition() { return Position; }

protected:

	uint SourceID = -1;

	bool bPositionChanged = true;
	Vector Position = Vector(0, 0, 0);
	Vector Velocity = Vector(0, 0, 0);


	bool bSettingChanged = true;
	float Pitch = 1.0f;
	float Gain = 1.0f;
	bool bLoop = false;
	bool bSourceRelative = false;

	bool bStartPlaying = false;
	bool bStopPlaying = false;
};
