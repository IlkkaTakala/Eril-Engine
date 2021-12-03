#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include <vector>
#include "ECS/EntityManager.h"

class System
{
	friend class SystemsManager;

protected:
	enum class SystemType { System = 0, IEntityQuerySystem, IComponentArrayQuerySystem };

public:
	System(EntityManager* entityManager) : WorldEntityManager(entityManager) { bEnabled = false; }
	~System() {}

	virtual void Update(float deltaTime) {};

	/// <summary>
	/// Set System to be enabled or disabled.
	/// </summary>
	/// <param name="enabled">True/False</param>
	void SetEnabled(bool enabled) { bEnabled = enabled; }
	void SetID(int id) { ID = id; }
	int GetID() { return ID; }

	void SetType(SystemType type) { Type = type; }
	SystemType GetType() { return Type; }
	
protected:

	bool bEnabled;
	int ID = -1;
	EntityManager* WorldEntityManager;
	std::vector<int> TypesWanted;
	std::vector<int> TypesNotAllowed;

	
	SystemType Type = SystemType::System;
};


class IEntityQuerySystem : public System
{
public:
	IEntityQuerySystem(EntityManager* entityManager) : System(entityManager) { SetType(SystemType::IEntityQuerySystem); }

	/// <summary>
	/// This is called once per frame.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	/// <param name="entities">Entities to go through</param>
	virtual void EntityQueryUpdate(float deltaTime, std::vector<int> entities) = 0;
};

template <class T>
class IComponentArrayQuerySystem : public System
{
public:
	IComponentArrayQuerySystem(EntityManager* entityManager, ComponentManager* componentManager) : System(entityManager), WorldComponentManager(componentManager)
	{
		SetType(SystemType::IComponentArrayQuerySystem);
		SystemComponentReferenceEntity = entityManager->AddEntity();
	}

	virtual void Update(float deltaTime) = 0;

	/// <summary>
	/// This is called once per frame.
	/// </summary>
	/// <param name="deltaTime">Time between frames</param>
	/// <param name="entities">Entities to go through</param>
	virtual void ComponentArrayQueryUpdate(float deltaTime, std::vector<T>* components) = 0;

	T* AddComponentToSystem(String componentType)
	{
		Console::Log("TEST");
		return WorldEntityManager->AddComponentToEntity<T>(SystemComponentReferenceEntity->GetID(), componentType);
	}

	bool RemoveComponentFromSystem(String componentType, int componentID)
	{
		return WorldEntityManager->RemoveComponentFromEntity<T>(SystemComponentReferenceEntity->GetID(), componentType, componentID);
	}

	std::vector<T>* GetComponentVector(String typeName)
	{
		return WorldComponentManager->GetComponentVector<T>(WorldComponentManager->GetTypeIdByName(typeName));
	}

private:
	Entity* SystemComponentReferenceEntity;
	ComponentManager* WorldComponentManager;
};
