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

	/// <summary>
	/// Adds a new entity to the ECS World
	/// </summary>
	/// <returns>The created Entity</returns>
	Entity* AddEntity();
	Entity* GetEntity(int entityIndex);
	bool RemoveEntity(Entity*& entity);
	bool RemoveEntity(int entityIndex);
	
	template <typename T>
	T* AddComponentToEntity(int entityIndex, String typeName)
	{
		int typeID = WorldComponentManager->GetTypeIdByName(typeName);
		if (entityIndex < static_cast<int>(IndexUsage.size()))
		{
			if (IndexUsage.at(entityIndex))
			{
				T* c = WorldComponentManager->AddComponent<T>(typeID);
				Entities.at(entityIndex)->GetComponents().insert(std::pair<int, int>(c->GetID(), typeID));
				return c;
			}
		}
		return nullptr;
	}

	template <typename T>
	T* GetComponentFromEntity(int entityIndex, String componentType)
	{
		int typeID = WorldComponentManager->GetTypeIdByName(componentType);

		Entity* entity = GetEntity(entityIndex);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetComponents())
			{
				if (c.second == typeID)
				{

					//ERRORI TAPAHTUU TÄSSÄ, JOKO KOMPONENTIN HAKU ENTITYN SISÄLTÄ EI TOIMI TAI SITTEN SITÄ EI VARASTOIDA ENTITYN SISÄLLE OIKEIN!
					Console::Log("GetComponentFromEntity " + std::to_string(entity->GetID()) + ", ID: " + std::to_string(entity->GetComponents().at(c.first)) + ", Type: " + std::to_string(Entities.at(entityIndex)->GetComponents().begin()->second));
					T* component = WorldComponentManager->GetComponent<T>(entity->GetComponents().at(c.first), typeID);
					if (component != nullptr)
					{
						//Console::Log(std::to_string(component->GetID()));
						return component;
					}
					break;
				}
			}
		}
		return nullptr;
	}


	//Component* CreateNewComponentToEntity(int entityIndex, String componentType);
	//Component* CreateNewComponentToEntity(int entityIndex, int componentTypeID);
	
	//Component* GetComponentFromEntity(int entityIndex, String componentType);
	bool RemoveComponentFromEntity(int entityIndex, String componentType);

	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);

	int GetTypeIdByName(String typeName);
private:

	ComponentManager* WorldComponentManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

