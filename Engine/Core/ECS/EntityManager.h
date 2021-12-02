#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
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

	/// <summary>
	/// Finds an entity with given ID
	/// </summary>
	/// <param name="entityID">ID of the Entity</param>
	/// <returns>Pointer to the entity if found.</returns>
	Entity* GetEntity(int entityID);

	/// <summary>
	/// Removes given entity from the world. Sets the given pointer to nullptr.
	/// </summary>
	/// <param name="entity">Pointer to the Entity to be removed</param>
	/// <returns>True if removing was successful</returns>
	bool RemoveEntity(Entity*& entity);

	/// <summary>
	/// Removes given entity from the world.
	/// </summary>
	/// <param name="entityID">ID of the entity.</param>
	/// <returns>True if removing was successful</returns>
	bool RemoveEntity(int entityID);

	/// <summary>
	/// Finds all the entities that meet the given requirements
	/// </summary>
	/// <param name="allowedTypes">Vector of allowed types</param>
	/// <returns>Entity ID:s that met the requirements</returns>
	std::vector<int> QueryEntitiesByType(std::vector<int> allowedTypes);

	/// <summary>
	/// Return typeName as an integer.
	/// </summary>
	/// <param name="typeName">Container's TypeName</param>
	/// <returns>Container's type as int</returns>
	int GetTypeIdByName(String typeName);

	/// <summary>
	/// Adds a new component to an Entity. Returns nullptr if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="componentTypeName">Component's type as string</param>
	/// <returns>Pointer to added Component</returns>
	template <typename T>
	T* AddComponentToEntity(int entityID, String componentTypeName)
	{
		int typeID = WorldComponentManager->GetTypeIdByName(componentTypeName);
		if (entityID < static_cast<int>(IndexUsage.size()))
		{
			if (IndexUsage.at(entityID))
			{
				T* c = WorldComponentManager->CreateComponent<T>(typeID);
				Entities.at(entityID)->GetComponents().insert(std::pair<int, int>(c->GetID(), typeID));
				return c;
			}
		}
		return nullptr;
	}

	/// <summary>
	/// Gets a component from an Entity. Returns nullptr if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="componentTypeName">Component's type as string</param>
	/// <returns>Pointer to Component if found</returns>
	template <typename T>
	T* GetComponentFromEntity(int entityID, String componentTypeName)
	{
		int typeID = WorldComponentManager->GetTypeIdByName(componentTypeName);

		Entity* entity = GetEntity(entityID);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetComponents())
			{
				if (c.second == typeID)
				{				
					T* component = WorldComponentManager->GetComponent<T>(c.first, typeID);
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

	/// <summary>
	/// Removes a component from an Entity. Returns false if something goes wrong.
	/// </summary>
	/// <typeparam name="T">Component type</typeparam>
	/// <param name="entityID">ID of the entity</param>
	/// <param name="componentTypeName">Component's type as string</param>
	/// <returns>True if removing was successful</returns>
	template <typename T>
	bool RemoveComponentFromEntity(int entityID, String componentTypeName)
	{
		int typeID = WorldComponentManager->GetTypeIdByName(componentTypeName);
		Entity* entity = GetEntity(entityID);
		if (entity != nullptr)
		{
			for (auto& c : entity->GetComponents())
			{
				if (c.second == typeID)
				{
					if (WorldComponentManager->RemoveComponent<T>(c.first, typeID))
					{
						//Console::Log("Removed Component of type" + std::to_string(typeID) + " from Entity " + std::to_string(entity->GetID()));
						entity->GetComponents().erase(c.first);
						return true;
					}
				}
			}
		}
		return false;
	}
	
private:

	ComponentManager* WorldComponentManager;

	std::vector<Entity*> Entities;
	std::vector<bool> IndexUsage;
};

