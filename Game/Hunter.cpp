#include "Hunter.h"
#include "Material.h"
#include <Objects/MovementComponent.h>
#include <Timer.h>
#include <Gameplay/GameState.h>
#include <Gameplay/PlayerController.h>

Hunter::Hunter()
{
	SetModel(MI->LoadData(this, "hunter"));
	RenderData->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/hunter"));

	move = SpawnObject<MovementComponent>();
	move->SetTarget(this);
	move->SetGravity(true);
	move->SetMaxSpeed(7.f);
	caught = false;

	time = 0.f;

	Timer::CreateTimer(5.f, &Hunter::SetNewTarget, this, true);
}

void Hunter::Tick(float delta)
{
	time += delta;
	RenderData->GetMaterial(0)->SetParameter("delta", time);
	RenderData->GetMaterial(0)->SetParameter("velocity", move->DesiredState.velocity);


	Vector player = GetGameState()->CurrentPlayer->GetLocation();
	
	if (!caught) {
		if ((targetLoc - Location).Length() < 1.f) SetNewTarget(0.f);
		move->AddInput(targetLoc - Location);

		if ((player - Location).Length() < 1.f) {
			caught = true;
			printf("Player caught, game over!\n");
			move->SetAllowMovement(false);
		}
	}
	else {
		SetLocation(player + Vector(0.f, 0.f, 1.f));
	}

}

void Hunter::SetNewTarget(float delta)
{
	printf("New target set\n");
	targetLoc = GetGameState()->CurrentPlayer->GetLocation();
}
