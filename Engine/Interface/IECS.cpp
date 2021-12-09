/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
08.12.2021
*/

#include "IECS.h"

//
#include <vector>
#include <Core.h>


//In-Engine Components and systems
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/LightComponent.h>
#include <ECS/Systems/AudioControllerSystem.h>
#include <ECS/Components/AudioComponent.h>

namespace IECS
{
	namespace
	{
		ComponentManager* WorldComponentManager;
		EntityManager* WorldEntityManager;
		SystemsManager* WorldSystemsManager;
	}

	void Init()
	{
		WorldComponentManager = new ComponentManager();
		WorldEntityManager = new EntityManager(WorldComponentManager);
		WorldSystemsManager = new SystemsManager(WorldEntityManager, WorldComponentManager);

		//In-engine component, entity and system setup
		//Light System:
		WorldComponentManager->CreateComponentTypeStorage<LightComponent>("LightComponent");
		LightControllerSystem* lightController = new LightControllerSystem(WorldEntityManager, WorldComponentManager, "LightComponent");
		int systemIndex = WorldSystemsManager->AddSystem(lightController, "LightControllerSystem");
		WorldSystemsManager->AddWantedComponentType(systemIndex, "LightComponent");

		//Audio System:
		WorldComponentManager->CreateComponentTypeStorage<AudioComponent>("AudioComponent");
		AudioControllerSystem* audioController = new AudioControllerSystem(WorldEntityManager, WorldComponentManager, "AudioComponent");
		systemIndex = WorldSystemsManager->AddSystem(audioController, "AudioControllerSystem");
		WorldSystemsManager->AddWantedComponentType(systemIndex, "AudioComponent");
	}

	void Destroy()
	{
		WorldSystemsManager->DestroySystems();
		WorldEntityManager->ClearEntities();
		WorldComponentManager->ResetStorages();
		delete WorldComponentManager;
		delete WorldEntityManager;
		delete WorldSystemsManager;
	}

	void ResetECSWorld()
	{
		WorldSystemsManager->DestroySystems();
		WorldEntityManager->ClearEntities();
		WorldComponentManager->ResetStorages();
		WorldSystemsManager->InitSystems();
	}

	ComponentManager* GetComponentManager()
	{
		return WorldComponentManager;
	}

	EntityManager* GetEntityManager()
	{
		return WorldEntityManager;
	}

	SystemsManager* GetSystemsManager()
	{
		return WorldSystemsManager;
	}
}