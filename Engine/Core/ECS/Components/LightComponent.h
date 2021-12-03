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

	LightComponent(LightComponent&& c) noexcept : Component(std::move(c))
	{
		LightType = std::move(c.LightType);
		Intensity = std::move(c.Intensity);
		Size = std::move(c.Size);
		Distance = std::move(c.Distance);
		Falloff = std::move(c.Falloff);
		Color = std::move(c.Color);
		Location = std::move(c.Location);
		Rotation = std::move(c.Rotation);
		State = std::move(c.State);
	}

	LightComponent& operator=(const LightComponent& c) 
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
		return *this; 
	}

	int LightType = 0;
	float Intensity = 2.f;
	float Size = 3.f;
	float Distance = 0.f;
	float Falloff = 1.f;
	Vector Color = Vector(1.f, 1.f, 1.f);

	Vector Location = Vector(0.f, 0.f, 0.f);
	Vector Rotation = Vector(0.f, 0.f, 0.f);

	enum class State { NotInitialized = 0, Enabled, NeedsDisabling, Disabled, NeedsEnabling };
	State State = State::NotInitialized;
};
