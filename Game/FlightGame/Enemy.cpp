#include "Enemy.h"
#include "Material.h"
#include <Objects/Terrain.h>
#include <Objects/MovementComponent.h>
#include <Timer.h>
#include <Gameplay/GameState.h>
#include <Gameplay/PlayerController.h>
#include <Objects/VisibleObject.h>
#include "FlightPlayer.h"

Enemy::Enemy() : Actor()
{
	Mesh = SpawnObject<VisibleObject>();
	AddComponent(Mesh);
	Mesh->SetModel("ghost");
	Mesh->GetModel()->SetMaterial(2, RI->LoadMaterialByName("Assets/Materials/ghost"));
	Mesh->GetModel()->SetMaterial(1, RI->LoadMaterialByName("Assets/Materials/ghost"));
	Mesh->GetModel()->SetMaterial(0, RI->LoadMaterialByName("Assets/Materials/ghost"));

	move = SpawnObject<MovementComponent>();
	move->SetTarget(this);
	move->SetGravity(true);
	move->SetMaxSpeed(3.0f);
	caught = false;
	
	time = 0.f;

	Timer::CreateTimer(1.f, &Enemy::SetNewTarget, this, true);
}

void Enemy::Tick(float delta)
{
	time += delta;
	Mesh->GetModel()->GetMaterial(0)->SetParameter("delta", time);
	Mesh->GetModel()->GetMaterial(0)->SetParameter("velocity", move->DesiredState.velocity);

	Vector playerLoc = GetGameState()->CurrentPlayer->GetLocation();

	if (!caught) {

		if ((targetLoc - Location).Length() < 1.f) SetNewTarget(0.f);
		move->AddInput(targetLoc - Location);

		if ((playerLoc - Location).Length() < 2.f) {
			caught = true;
			Console::Log("Player caught, game over!");
			dynamic_cast<FlightPlayer*>(GetGameState()->CurrentPlayer.GetPointer())->Caught();
			move->SetAllowMovement(false);
		}
	}
	else {
		SetLocation(playerLoc + Vector(0.f, 0.f, 1.f));
	}
}

void Enemy::stopMoving()
{
	move->SetAllowMovement(false);
}

void Enemy::startMoving()
{
	move->SetAllowMovement(true);
}

void Enemy::SetNewTarget(float delta)
{
	if (GetGameState()->CurrentPlayer != nullptr)
		targetLoc = GetGameState()->CurrentPlayer->GetLocation();
}

