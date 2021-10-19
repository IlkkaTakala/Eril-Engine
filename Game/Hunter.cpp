#include "Hunter.h"
#include "Material.h"
#include <Objects/MovementComponent.h>

Hunter::Hunter()
{
	SetModel(MI->LoadData(this, "hunter"));
	RenderData->SetMaterial(0, RI->LoadMaterialByName("Shaders/Materials/hunter"));

	move = SpawnObject<MovementComponent>();
	move->SetTarget(this);
	move->SetGravity(false);

	time = 0.f;
}

void Hunter::Tick(float delta)
{
	time += delta;
	RenderData->GetMaterial(0)->SetParameter("delta", time);
	RenderData->GetMaterial(0)->SetParameter("velocity", move->DesiredState.velocity);
}
