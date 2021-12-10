#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include <ECS/SystemsManager.h>
#include <ECS/Components/LightComponent.h>

constexpr int LIGHT_DIRECTIONAL = 0;
constexpr int LIGHT_POINT = 1;

class LightControllerSystem : public IComponentArrayQuerySystem<LightComponent>
{
public:
	LightControllerSystem(EntityManager* entityManager, ComponentManager* componentManager, String componentTypeName) : IComponentArrayQuerySystem<LightComponent>(entityManager, componentManager, componentTypeName) {}


	void Update(float deltaTime)
	{
		
		ComponentArrayQueryUpdate(deltaTime, GetComponentVector());
	}

	void ComponentArrayQueryUpdate(float deltaTime, std::vector<LightComponent>* components)
	{
		for (LightComponent& light : *components)
		{
			if (!light.GetDisabled())
			{
				int s = static_cast<int>(light.State);
			}
		}
	}

private:
};
