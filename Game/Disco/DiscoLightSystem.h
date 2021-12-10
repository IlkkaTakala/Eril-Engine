#pragma once


#include <ECS/SystemsManager.h>
#include <ECS/Components/LightComponent.h>
#include <Disco/DiscoLightComponent.h>


class DiscoLightSystem : public IComponentArrayQuerySystem<DiscoLightComponent>
{
public:
	DiscoLightSystem(EntityManager* entityManager, ComponentManager* componentManager, String componentTypeName) : IComponentArrayQuerySystem<DiscoLightComponent>(entityManager, componentManager, componentTypeName) 
	{
		WorldComponentManager = componentManager;
	}


	void Update(float deltaTime)
	{

		ComponentArrayQueryUpdate(deltaTime, GetComponentVector());
	}

	void ComponentArrayQueryUpdate(float deltaTime, std::vector<DiscoLightComponent>* components)
	{
		for (DiscoLightComponent& light : *components)
		{
			if (!light.GetDisabled())
			{
				float rad = 1.0f;
				float s = light.s += deltaTime / 2.0f;
				float t = light.t += deltaTime / 2.0f;


				float x = rad * cos(s) * sin(t);
				float y = rad * sin(s) * sin(t);
				float z = rad * cos(t);

				Vector loc(x, y, z);
				loc *= Vector(24, 45, 16);
				loc.Z += 16;


				//int s = static_cast<int>(light.State);

				LightComponent &lightComponent =
					WorldComponentManager->GetComponentVector<LightComponent>(WorldComponentManager->GetTypeIdByName("LightComponent"))->at(light.LightID);
				lightComponent.Location = loc;
			}
		}
	}

private:
	ComponentManager* WorldComponentManager;

};


