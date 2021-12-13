#pragma once
#include <Core.h>

class VisibleObject;
class MovementComponent;

namespace Physics
{
	void CheckCollisions(float delta);
	void AddStatic(VisibleObject* obj);
	void RemoveStatic(VisibleObject* obj);
	void AddMovable(MovementComponent* obj);
	void RemoveMovable(MovementComponent* obj);
};

