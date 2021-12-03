#include "Hunter.h"
#include "Material.h"
#include <Objects/Terrain.h>
#include <Objects/MovementComponent.h>
#include <Timer.h>
#include <Gameplay/GameState.h>
#include <Gameplay/PlayerController.h>
#include <Objects/VisibleObject.h>


Hunter::Hunter() : Actor()
{
	Mesh = SpawnObject<VisibleObject>();
	AddComponent(Mesh);
	Mesh->SetModel("ghost");
	Mesh->GetModel()->SetMaterial(2, RI->LoadMaterialByName("Assets/Materials/hunter"));
	Mesh->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/hunter"));
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/hunter"));

	move = SpawnObject<MovementComponent>();
	move->SetTarget(this);
	move->SetGravity(true);
	move->SetGround(ObjectManager::GetByRecord<Terrain>(0xA0001111));
	move->SetMaxSpeed(1.f);
	caught = false;

	time = 0.f;

	Timer::CreateTimer(5.f, &Hunter::SetNewTarget, this, true);
}

void Hunter::Tick(float delta)
{
	time += delta;
	Mesh->GetModel()->GetMaterial(0)->SetParameter("delta", time);
	Mesh->GetModel()->GetMaterial(0)->SetParameter("velocity", move->DesiredState.velocity);

	Vector playerLoc = GetGameState()->CurrentPlayer->GetLocation();
	
	if (!caught) {
		//SetRotation(Rotation.RotateByAxis(Location, playerLoc.X, 90.f));

		if ((targetLoc - Location).Length() < 1.f) SetNewTarget(0.f);
		move->AddInput(targetLoc - Location);

		if ((playerLoc - Location).Length() < 1.f) {
			caught = true;
			Console::Log("Player caught, game over!");
			move->SetAllowMovement(false);
		}
	}
	else {
		SetLocation(playerLoc + Vector(0.f, 0.f, 1.f));
	}
}

void Hunter::SetNewTarget(float delta)
{
	Console::Log("New target set");
	if (GetGameState()->CurrentPlayer != nullptr)
		targetLoc = GetGameState()->CurrentPlayer->GetLocation();
}

