#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "Core.h"
#include "ECS/Component.h"

struct LightComponent : public Component
{
	LightComponent(){}
	LightComponent(const LightComponent& c) : Component(static_cast<Component>(c)) 
	{ 
		LightType = c.LightType;
		Intensity = c.Intensity;
		Size = c.Size;
		Distance = c.Distance;
		Falloff = c.Falloff;
		Color = c.Color;
		Location = c.Location;
		Rotation = c.Rotation;
		State = c.State;
	}


	int LightType = 0;
	float Intensity = 2.f;
	float Size = 3.f;
	float Distance = 0.f;
	float Falloff = 1.f;
	Vector Color = Vector(1.f, 1.f, 1.f);

	Vector Location = Vector(0.f, 0.f, 0.f);
	Vector Rotation = Vector(0.f, 0.f, 0.f);

	// 0 = Not initialized
	// 1 = Enabled
	// 3 = Enabled, Needs Disabling		
	// 4 = Disabled
	// 5 = Disabled, Needs Enabling
	int State = 0;
};
