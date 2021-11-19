#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021 Last Edited by RabbitTortoise 18.11.2021
*/

#include "ECS/Component.h"

class PositionComponent : public Component
{
public:
	PositionComponent() : Component() {}
	PositionComponent(const PositionComponent& c) : Component() { X = c.X; Y = c.Y; Z = c.Z; }

	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
};
