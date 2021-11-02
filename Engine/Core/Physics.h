#pragma once
#include <Core.h>

class VisibleObject;
class MovementComponent;

namespace Physics
{
	void CheckCollisions();
	void AddStatic(VisibleObject* obj);
	void RemoveStatic(VisibleObject* obj);
	void AddMovable(MovementComponent* obj);
	void RemoveMovable(MovementComponent* obj);
};

