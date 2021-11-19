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

	void Update(float deltaTime, std::vector<int> entities)
	{
		if (!done)
		{
			Console::Log("Size" + std::to_string(entities.size()));
			done = true;



			for (auto e : entities)
			{
				PositionComponent* pos = static_cast<PositionComponent*>(WorldEntityManager.GetComponentFromEntity(e, "PositionComponent"));
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
	}


private:
	bool done = false;
};
