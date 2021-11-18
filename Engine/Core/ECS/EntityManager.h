#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 18.11.2021
*/

#include <Core.h>
#include <ECS/Entity.h>
#include <ECS/ComponentManager.h>

/// <summary>
/// Contains all the entities in the ECS World and has functionality to add, get and remove Entities, to add, get and remove Comonents to/from Entities
/// and to query vectors of Entities based on different requirements.
/// </summary>
class EntityManager
{
public:
	EntityManager(ComponentManager* componentManager) : WorldComponentManager(componentManager) {}

	Entity* AddEntity();
	Entity* GetEntity(int entityIndex);
	bool RemoveEntity(Entity*& entity);
	bool RemoveEntity(int entityIndex);
	

	int CreateNewComponentToEntity(int entityIndex, String componentType);
	int CreateNewComponentToEntity(int entityIndex, int componentTypeID);
	
	Component* GetComponentFromEntity(int entityIndex, String componentType);
	bool RemoveComponentFromEntity(int entityIndex, String componentType);

	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);

	int GetTypeIdByName(String typeName);
private:

	ComponentManager* WorldComponentManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

