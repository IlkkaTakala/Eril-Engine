#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class MovementComponent;

class FallingCube : public Actor
{
public:
	FallingCube();

private:
	Ref<MovementComponent> move;
};

