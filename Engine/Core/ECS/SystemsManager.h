#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/
#include "Core.h"
#include "ECS/EntityManager.h"
#include "ECS/System.h"

class SystemsManager
{
	friend class ECSWorldManager;
public:
	SystemsManager(EntityManager& entityManager, ComponentManager& componentManager) 
		: WorldEntityManager(entityManager), WorldComponentManager(componentManager) {}
	int AddSystem(System* system);
	bool DisableSystem(int systemIndex);
	bool AddWantedComponentType(int systemIndex, String componentType);

	void UpdateSystems(float deltaTime);


protected:
	EntityManager& WorldEntityManager;
	ComponentManager& WorldComponentManager;
	std::vector<System*> Systems;
};

