/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
12.11.2021
*/

#include "IECS.h"

//
#include <vector>
#include <Core.h>


//In-Engine Components and systems
#include <ECS/Systems/LightControllerSystem.h>
#include <ECS/Components/LightComponent.h>

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
		LightControllerSystem* lightController = new LightControllerSystem(*WorldEntityManager);
		int systemIndex = WorldSystemsManager->AddSystem(lightController);
		WorldSystemsManager->AddWantedComponentType(systemIndex, "LightComponent");
	}

	void Destroy()
	{
		delete WorldComponentManager;
		delete WorldEntityManager;
		delete WorldSystemsManager;
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