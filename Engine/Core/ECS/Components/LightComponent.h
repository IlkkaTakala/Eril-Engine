/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
11.11.2021
*/

#include "Core.h"
#include "ECS/Component.h"

//#include "IRender.h"




struct LightComponent : public Component
{
	LightComponent() : Component() {}
	int Type = 0;
	float Intensity = 2.f;
	float Size = 3.f;
	float Distance = 0.f;
	float Falloff = 1.f;
	Vector Color = Vector(1.f, 1.f, 1.f);

	Vector Location = Vector(0.f, 0.f, 0.f);
	Vector Rotation = Vector(0.f, 0.f, 0.f);
};
