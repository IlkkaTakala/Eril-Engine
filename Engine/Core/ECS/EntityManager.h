#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 18.11.2021
*/

#include <Core.h>
#include <ECS/Entity.h>
#include <ECS/ComponentManager.h>
#include <algorithm>

class EntityManager
{
public:
	EntityManager(ComponentManager& componentManager) : WorldComponentManager(componentManager) {}

	Entity* AddEntity();
	Entity* GetEntity(int entityIndex);
	bool RemoveEntity(int entityIndex);
	bool RemoveEntity(Entity& entity);

	bool AddComponentToEntity(int entityIndex, Component& component, String typeName)
	{
		if (entityIndex < static_cast<int>(IndexUsage.size()))
		{
			if (IndexUsage.at(entityIndex))
			{
				int componentID = WorldComponentManager.AddComponent(component, typeName);
				int componentType = WorldComponentManager.GetTypeIdByName(typeName);

				Entities.at(entityIndex)->GetComponents().insert(std::pair<int, int>(componentID, componentType));
				//Entities.at(entityIndex)->GetComponents().push_back(componentID);
				//Entities.at(entityIndex)->GetComponentTypes().push_back(componentType);
				return true;
			}
		}
		return false;
	}
	//template <typename T>
	Component* GetComponentFromEntity(int entityIndex, String componentType)
	{
		int typeID = WorldComponentManager.GetTypeIdByName(componentType);

		Entity* entity = GetEntity(entityIndex);
		if (entity != nullptr)
		{

			for (auto &c : entity->GetComponents())
			{
				if (c.second == typeID)
				{
					Component* component = WorldComponentManager.GetComponent(entity->GetComponents().at(c.first), componentType);
					if (component != nullptr)
					{
						return component;
					}
					break;
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

