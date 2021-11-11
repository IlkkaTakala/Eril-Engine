#pragma once

#include "ECS/Component.h"

class PositionComponent : public Component
{
public:
	PositionComponent() : Component() {}
	float x = 0;
	float y = 0;
	float z = 0;
};
