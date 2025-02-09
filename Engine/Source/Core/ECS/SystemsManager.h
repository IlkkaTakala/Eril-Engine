#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/
#include "Core.h"
#include "ECS/EntityManager.h"
#include "ECS/System.h"

class SystemsManager
{
public:
	SystemsManager(EntityManager* entityManager, ComponentManager* componentManager) 
		: WorldEntityManager(entityManager), WorldComponentManager(componentManager) {}

	/// <summary>
	/// Adds a new system to be updated each frame.
	/// </summary>
	/// <param name="system">Pointer to system to be added.</param>
	/// <returns>System's ID</returns>
	int AddSystem(System* system, String systemName);

	/// <summary>
	/// Disables the system with given ID.
	/// </summary>
	/// <param name="systemID">ID of the system</param>
	/// <returns>True if operation was successful</returns>
	bool DisableSystem(int systemID);

	/// <summary>
	/// Adds a wanted component type to the system so it processess entities with those components.
	/// </summary>
	/// <param name="systemID">ID of the system</param>
	/// <param name="componentTypeName">Component's type as string</param>
	/// <returns>True if operation was successful</returns>
	bool AddWantedComponentType(int systemID, String componentTypeName);

	/// <summary>
	/// Updates all systems.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	void UpdateSystems(float deltaTime);


	System* GetSystemByName(String systemName);
	
	void DestroySystems();
	void InitSystems();

protected:
	EntityManager* WorldEntityManager;
	ComponentManager* WorldComponentManager;
	std::vector<System*> Systems;
	
	/// <summary>
	/// First: System Name, Second: System ID
	/// </summary>
	std::map<String, int> SystemNames;
};

