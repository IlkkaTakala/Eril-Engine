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

	virtual void Init() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Destroy() = 0;
	

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
	IEntityQuerySystem(EntityManager* entityManager) : System(entityManager) 
	{
		SetType(SystemType::IEntityQuerySystem);
	}

	virtual void Init()
	{
	}

	virtual void Destroy()
	{
	}
	virtual void Update(float deltaTime) = 0;
	virtual void EntityQueryUpdate(float deltaTime, std::vector<int> entities) = 0;
};

template <class T>
class IComponentArrayQuerySystem : public System
{
public:
	IComponentArrayQuerySystem(EntityManager* entityManager, ComponentManager* componentManager, String typeName) : System(entityManager)
	{
		SetType(SystemType::IComponentArrayQuerySystem);
		WorldComponentManager = componentManager;
		ComponentTypeName = typeName;
		Init();
	}

	virtual void Init()
	{
		SystemComponentReferenceEntity = WorldEntityManager->AddEntity();
		SystemComponentReferenceEntity->SetProtected(true);
	}

	virtual void Destroy()
	{
		SystemComponentReferenceEntity->SetProtected(false);
		WorldEntityManager->RemoveEntity(SystemComponentReferenceEntity);
	}

	virtual void Update(float deltaTime) = 0;
	virtual void ComponentArrayQueryUpdate(float deltaTime, std::vector<T>* components) = 0;

	T* AddComponentToSystem()
	{
		return WorldEntityManager->AddComponentToEntity<T>(SystemComponentReferenceEntity->GetID(), ComponentTypeName);
	}

	virtual bool RemoveComponentFromSystem(int componentID)
	{
		return WorldEntityManager->RemoveComponentFromEntity<T>(SystemComponentReferenceEntity->GetID(), ComponentTypeName, componentID);
	}

	std::vector<T>* GetComponentVector()
	{
		return WorldComponentManager->GetComponentVector<T>(WorldComponentManager->GetTypeIdByName(ComponentTypeName));
	}

private:
	Entity* SystemComponentReferenceEntity;
	ComponentManager* WorldComponentManager;
	String ComponentTypeName;
};



/* Example System
#pragma once


#include "ECS/SystemsManager.h"
#include "Components/PositionComponent.h"

class MovementSystem : public IEntityQuerySystem
{
public:
	MovementSystem(EntityManager* entityManager) : IEntityQuerySystem(entityManager)
	{
		Console::Log("MovementSystem Constructor");
	}

	void Update(float deltaTime)
	{
		EntityQueryUpdate(deltaTime, WorldEntityManager->QueryEntitiesByType(TypesWanted));
	}


	void EntityQueryUpdate(float deltaTime, std::vector<int> entities)
	{
		if (!Done)
		{
			Done = true;
			Console::Log("MovementSystem: Entity count: " + std::to_string(entities.size()));
		}
		for (auto e : entities)
		{
			//Console::Log("Processing entity " + std::to_string(WorldEntityManager.GetEntity(e)->GetID()));
			PositionComponent* pos = WorldEntityManager->GetComponentFromEntity<PositionComponent>(WorldEntityManager->GetEntity(e)->GetID(), "PositionComponent");
			if (pos != nullptr)
			{
				pos->X += 10.0f * deltaTime;
				pos->Y += 10.0f * deltaTime;
				pos->Z += 10.0f * deltaTime;

				//Console::Log(std::to_string(WorldEntityManager.GetEntity(e)->GetID()) + "," + std::to_string(pos->GetID()) + "," + std::to_string(pos->Z) + "," + std::to_string(pos->Y) + "," + std::to_string(pos->Z));
			}
			else
			{
				Console::Log("Component NOT FOUND!");
			}
		}

	}


private:
	bool Done = false;
};
*/