#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "ECS/SystemsManager.h"
#include "ECS/Components/LightComponent.h"

class LightControllerSystem : public IComponentArrayQuerySystem<LightComponent>
{
public:
	LightControllerSystem(EntityManager* entityManager, ComponentManager* componentManager) : IComponentArrayQuerySystem<LightComponent>(entityManager, componentManager) {}

	void Update(float deltaTime)
	{
		
		ComponentArrayQueryUpdate(deltaTime, GetComponentVector("LightComponent"));
	}

	void ComponentArrayQueryUpdate(float deltaTime, std::vector<LightComponent>* components)
	{
		if (!done)
		{
			done = true;
			for (LightComponent& light : *components)
			{
				int s = static_cast<int>(light.State);
				Console::Log("Light State " + std::to_string(s));
			}

		}
	}


private:
	bool done = false;
};
