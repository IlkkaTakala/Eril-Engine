#pragma once

#include "ECS/Component.h"

struct PositionComponent : public Component
{
	PositionComponent() : Component() {}
	float x = 0;
	float y = 0;
	float z = 0;
};
