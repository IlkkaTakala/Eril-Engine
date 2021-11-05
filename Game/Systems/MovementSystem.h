#pragma once

#include "ECS/SystemsManager.h"
#include "Components/PositionComponent.h"

class MovementSystem : public System
{
public:
	MovementSystem(EntityManager& entityManager) : System(entityManager) {}

	void Update(float deltaTime, std::vector<int> entities) override
	{
		printf("Size: %d  ", entities.size());
		for (auto e : entities)
		{
			PositionComponent* pos = static_cast<PositionComponent*>(WorldEntityManager.GetComponentFromEntity(e, "PositionComponent"));
			if (pos != nullptr)
			{
				pos->x += 1.0f * deltaTime;
				pos->y += 1.0f * deltaTime;
				pos->z += 1.0f * deltaTime;

				printf("Entity %d position: %f,%f,%f\n", e, pos->x, pos->y, pos->z);
			}
			else
			{
				printf("Component NOT FOUND! \n");
			}
		}
	}


private:

};
