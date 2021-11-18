#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "ECS/SystemsManager.h"
#include "Components/PositionComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem(EntityManager& entityManager) : System(entityManager) 
	{
		Console::Log("MovementSystem Constructor");
	}

	void Update(float deltaTime, std::vector<int> entities) override
	{
		//Console::Log("Size" + std::to_string(entities.size()));

		for (auto e : entities)
		{
			PositionComponent* pos = static_cast<PositionComponent*>(WorldEntityManager.GetComponentFromEntity(e, "PositionComponent"));
			if (pos != nullptr)
			{
				pos->x += 1.0f * deltaTime;
				pos->y += 1.0f * deltaTime;
				pos->z += 1.0f * deltaTime;

				//Console::Log(std::to_string(pos->x) + "," + std::to_string(pos->y) + "," + std::to_string(pos->z));
			}
			else
			{
				Console::Log("Component NOT FOUND!");
			}
		}
	}


private:
};
