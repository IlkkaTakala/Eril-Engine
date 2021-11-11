#pragma once

#include "ECS/Component.h"

class PositionComponent : public Component
{
public:
	PositionComponent() : Component() { Init(); }
	PositionComponent(const PositionComponent& obj) { x = obj.x; y = obj.y; z = obj.z; }
	void Init() { x = 0.0f; y = 0.0f; z = 0.0f; }

	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
};
