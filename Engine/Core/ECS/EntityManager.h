#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021
*/

#include "Core.h"
#include "ECS/Entity.h"
#include "ECS/ComponentManager.h"
#include <algorithm>

class EntityManager
{
	friend class ECSWorldManager;
public:
	EntityManager(ComponentManager& componentManager) : WorldComponentManager(componentManager) {}

	Entity* AddEntity();
	Entity* GetEntity(int entityIndex);
	bool RemoveEntity(int entityIndex);
	bool RemoveEntity(Entity& entity);
	
	bool AddComponentToEntity(int entityIndex, Component& component, String typeName);


	Component* GetComponentFromEntity(int entityIndex, String componentType);

	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);
private:

	ComponentManager& WorldComponentManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

