#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "ECS/SystemsManager.h"
#include "ECS/Components/LightComponent.h"

class LightControllerSystem : public System
{
public:
	LightControllerSystem(EntityManager& entityManager) : System(entityManager) {}

	void Update(float deltaTime, std::vector<int> entities) override
	{
		printf("Size: %d   \n", entities.size());
		for (auto e : entities)
		{
			//
		}
	}


private:

};
