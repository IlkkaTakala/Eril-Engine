#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
07.12.2021
*/

#include <ECS/SystemsManager.h>
#include <ECS/Components/AudioComponent.h>
#include <Interface/AudioManager.h>


class AudioControllerSystem : public IComponentArrayQuerySystem<AudioComponent>
{
public:
	AudioControllerSystem(EntityManager* entityManager, ComponentManager* componentManager, String componentTypeName) : IComponentArrayQuerySystem<AudioComponent>(entityManager, componentManager, componentTypeName) {}


	void Update(float deltaTime)
	{

		ComponentArrayQueryUpdate(deltaTime, GetComponentVector());
	}

	void ComponentArrayQueryUpdate(float deltaTime, std::vector<AudioComponent>* components)
	{
		for (AudioComponent& audio : *components)
		{
			if (!audio.GetDisabled())
			{
				if (audio.bSettingChanged)
				{
					AudioManager::SetAudioPitch(audio.SourceID, audio.Pitch);
					AudioManager::SetAudioGain(audio.SourceID, audio.Gain);
					AudioManager::SetAudioLooping(audio.SourceID, audio.bLoop);
					AudioManager::SetAudioRelativity(audio.SourceID, audio.bSourceRelative);
					audio.bSettingChanged = false;
				}
				if (audio.bPositionChanged)
				{
					AudioManager::SetAudioPosition(audio.SourceID, audio.Position);
					AudioManager::SetAudioVelocity(audio.SourceID, audio.Velocity);
					audio.bPositionChanged = false;
				}
				if (audio.bStartPlaying)
				{
					AudioManager::PlayAudio(audio.SourceID);
					audio.bStartPlaying = false;
				}
				if (audio.bStopPlaying)
				{
					AudioManager::StopAudio(audio.SourceID);
					audio.bStopPlaying = false;
				}
			}
		}
	}

	uint LoadAudioFile(String fileName)
	{
		return AudioManager::LoadAudio(fileName);
	}


private:
};
