#pragma once


#include <Core.h>
#include <ECS/Component.h>

struct DiscoLightComponent : public Component
{
public:
	//friend class AudioControllerSystem;

	DiscoLightComponent()
	{

	}
	DiscoLightComponent(const DiscoLightComponent& c) : Component(static_cast<Component>(c))
	{
		LightID = c.LightID;

		s = c.s;
		t = c.t;
		

	}

	DiscoLightComponent(DiscoLightComponent&& c) noexcept : Component(std::move(c))
	{
		LightID = std::move(c.LightID);

		s = std::move(c.s);
		t = std::move(c.t);
		
	}

	DiscoLightComponent& operator=(const DiscoLightComponent& c)
	{
		LightID = c.LightID;

		s = c.s;
		t = c.t;

		return *this;
	}


public:

	int LightID = -1;
	float s = 1;
	float t = 1;



	
};
