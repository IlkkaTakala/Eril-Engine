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

	bool AddComponentToEntity(int entityIndex, Component& component, String typeName)
	{
		if (entityIndex < IndexUsage.size())
		{
			if (IndexUsage.at(entityIndex))
			{
				int componentID = WorldComponentManager.AddComponent(component, typeName);
				int componentType = WorldComponentManager.GetTypeByName(typeName);

				Entities.at(entityIndex)->GetComponents().push_back(componentID);
				Entities.at(entityIndex)->GetComponentTypes().push_back(componentType);
				return true;
			}
		}
		return false;
	}
	template <typename T>
	Component* GetComponentFromEntity(int entityIndex, String componentType, T type)
	{
		int typeID = WorldComponentManager.GetTypeByName(componentType);

		Entity* entity = GetEntity(entityIndex);
		if (entity != nullptr)
		{
			bool componentFound = false;
			int componentID = 0;

			for (auto c : entity->GetComponentTypes())
			{
				if (c == typeID)
				{
					componentFound = true;
					break;
				}
				componentID++;
			}
			if (componentFound)
			{
				Component* component = WorldComponentManager.GetComponent(componentID, componentType, type);
				if (component != nullptr)
				{
					return component;
				}
			}
		}
		return nullptr;
	}

	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);
private:

	ComponentManager& WorldComponentManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

