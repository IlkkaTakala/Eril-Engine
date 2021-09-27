#include <Objects/MovementComponent.h>
#include "FallingCube.h"

FallingCube::FallingCube()
{
	SetModel("Cube");
	GetModel()->SetMaterial(0, RI->LoadMaterialByName("Shaders/metal"));

	move = SpawnObject<MovementComponent>();
	move->SetGravity(true);
}