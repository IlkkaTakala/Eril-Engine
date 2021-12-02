#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "ECS/SystemsManager.h"
#include "ECS/Components/LightComponent.h"

class LightControllerSystem : public IEntityQuerySystem
{
public:
	LightControllerSystem(EntityManager& entityManager) : IEntityQuerySystem(entityManager) {}

	void Update(float deltaTime, std::vector<int> entities) override
	{
		
		for (auto e : entities)
		{
			//
		}
	}


private:

};
