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

class ECSWorldManager
{
public:
	ECSWorldManager();
	~ECSWorldManager();

	void Tick(float deltaTime);

	ComponentManager& GetComponentManager() { return *WorldComponentManager; };
	EntityManager& GetEntityManager() { return *WorldEntityManager; };
	SystemsManager& GetSystemsManager() { return *WorldSystemsManager; };

private:
	ComponentManager* WorldComponentManager;
	EntityManager* WorldEntityManager;
	SystemsManager* WorldSystemsManager;
};

