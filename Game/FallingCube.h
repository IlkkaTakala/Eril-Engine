#pragma once
#include <Core.h>
#include <Objects/Actor.h>

class MovementComponent;

class FallingCube : public Actor
{
public:
	FallingCube();
	virtual void Tick(float) override;

private:
	Ref<MovementComponent> move;
};

