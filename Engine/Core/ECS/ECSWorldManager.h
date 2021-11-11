#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include <vector>
#include "Core.h"
#include "ECS/ComponentManager.h"
#include "ECS/EntityManager.h"
#include "ECS/SystemsManager.h"
#include "ECS/Systems/LightControllerSystem.h"
#include "ECS/Components/LightComponent.h"

class ECSWorldManager
{
public:
	ECSWorldManager();
	~ECSWorldManager();

	void Update(float deltaTime);

	ComponentManager& GetComponentManager() { return *WorldComponentManager; };
	EntityManager& GetEntityManager() { return *WorldEntityManager; };
	SystemsManager& GetSystemsManager() { return *WorldSystemsManager; };

private:
	void SetupEngineSystems();

	ComponentManager* WorldComponentManager;
	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;
};

