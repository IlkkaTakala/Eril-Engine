#include <Objects/MovementComponent.h>
#include "FallingCube.h"

FallingCube::FallingCube()
{
	SetModel("Cube");
	GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/ground"));

	move = SpawnObject<MovementComponent>();
	move->SetTarget(this);
	move->SetGravity(true);
}

void FallingCube::Tick(float)
{
	if (GetLocation().Z < -50.f) DestroyObject();
}
