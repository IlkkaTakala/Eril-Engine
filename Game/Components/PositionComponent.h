#pragma once
/*
Author: Albert Uusi-Illikainen [RabbitTortoise]
5.11.2021 Last Edited by RabbitTortoise 24.11.2021
*/

#include "ECS/Component.h"

class PositionComponent : public Component
{
public:
	PositionComponent() : Component() {}
	PositionComponent(const PositionComponent &c) : Component(static_cast<Component>(c)), X(c.X), Y(c.Y), Z(c.Z) {}
	PositionComponent(PositionComponent &&c) noexcept : Component(std::move(c)), X(std::move(c.X)), Y(std::move(c.Y)), Z(std::move(c.Z)) {}

	PositionComponent& operator=(const PositionComponent& c) { X = c.X; Y = c.Y; Z = c.Z; ID = c.ID; Type = c.Type; Disabled = c.Disabled; return *this; }

	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
};
