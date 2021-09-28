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
	if (GetRecord() == 35) printf("%f\n", Location.Z);
	if (Location.Z < -10.f) DestroyObject();
}
