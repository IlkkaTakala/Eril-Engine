#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
09.12.2021, Last Edited by RabbitTortoise 24.11.2021
*/

#include "ECS/SystemsManager.h"
#include "Components/PositionComponent.h"

class MovementSystem : public IEntityQuerySystem
{
public:
	MovementSystem(EntityManager* entityManager) : IEntityQuerySystem(entityManager)
	{
		Console::Log("MovementSystem Constructor");
	}

	virtual void Init()
	{
		IEntityQuerySystem::Init();
	}

	virtual void Destroy()
	{
		IEntityQuerySystem::Destroy();
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
