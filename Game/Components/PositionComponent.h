#pragma once

#include "ECS/Component.h"

class PositionComponent : public Component
{
public:
	PositionComponent() : Component() {Console::Log("PositionComponent init()");x = 0.0f; y = 0.0f; z = 0.0f;}
	float x;
	float y;
	float z;
};
