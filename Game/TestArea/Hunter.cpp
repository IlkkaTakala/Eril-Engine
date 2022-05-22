#include "Hunter.h"
#include "Material.h"
#include <Objects/MovementComponent.h>
#include <Timer.h>
#include <Gameplay/GameState.h>
#include <Gameplay/PlayerController.h>
#include <Objects/SkeletalObject.h>
#include <SkeletalMesh.h>
#include <Objects/CollisionShape.h>

Hunter::Hunter() : Actor()
{
	Mesh = SpawnObject<SkeletalObject>(this);
	AddComponent(Mesh);
	Mesh->SetModel("Assets/Skeletal/Alien");
	Mesh->GetModel()->SetMaterial(1, IRender::LoadMaterialByName("Assets/Materials/alien_upper"));
	Mesh->GetModel()->SetMaterial(0, IRender::LoadMaterialByName("Assets/Materials/alien_lower"));
	Mesh->GetModel()->SetAABB(AABB(Vector(-1.f, -1.f, 0.f), Vector(1.f, 1.f, 2.f)));
	Mesh->SetRotation(90);
	Mesh->SetScale(0.01);
	auto animC = SpawnObject<HunterAnimControl>(this, Mesh);
	animC->SetSkeleton(Mesh->GetModel());
	Mesh->SetAnimController(animC);

	move = SpawnObject<MovementComponent>(this);
	move->SetTarget(this);
	move->SetGravity(true);
	move->SetMaxSpeed(2.f);
	move->SetRotateToVelocity(true);

	auto PlayerCol = SpawnObject<CapsuleCollisionShape>(this);
	AddComponent(PlayerCol);
	PlayerCol->SetLocation(Vector(0.f, 0.f, 1.f), true);
	PlayerCol->SetType(2);
	PlayerCol->SetSize(0.5, 1.f);
	PlayerCol->SetMovementTarget(move);

	caught = false;
	gun = true;

	time = 0.f;

	Timer::CreateTimer(1.f, &Hunter::SetNewTarget, this, true);
}

void Hunter::Tick(float delta)
{
	time += delta;
	Mesh->GetModel()->GetMaterial(0)->SetParameter("delta", time);
	Mesh->GetModel()->GetMaterial(0)->SetParameter("velocity", move->DesiredState.velocity);


	Vector player = GetGameState()->CurrentPlayer->GetLocation();

	if (!caught) {
		const Vector dir(targetLoc - Location);
		if (dir.Length() < 1.f) SetNewTarget(0.f);
		move->AddInput(dir);

		if ((player - Location).Length() < 1.f) {
			caught = true;
			Console::Log("Player caught, game over!");
			move->SetAllowMovement(false);
		}
	}
	else {
		SetLocation(player + Vector(0.f, 0.f, 1.f));
	}

}

void Hunter::SetNewTarget(float delta)
{
	if (GetGameState()->CurrentPlayer != nullptr)
		targetLoc = GetGameState()->CurrentPlayer->GetLocation();
}

void HunterAnimControl::BeginPlay()
{
	blender.AddKey(0.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleIdle", owner->GetModel()));
	blender.AddKey(2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleStrafeWalkRight", owner->GetModel()));
	blender.AddKey(-2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleStrafeWalkLeft", owner->GetModel()));
	blender.AddKey(0.f, 2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleWalk", owner->GetModel()));
	blender.AddKey(0.f, -2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleWalkBack", owner->GetModel()));
	blender.AddKey(4.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleStrafeRunRight", owner->GetModel()));
	blender.AddKey(-4.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleStrafeRunLeft", owner->GetModel()));
	blender.AddKey(0.f, 4.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleRun", owner->GetModel()));
	blender.AddKey(0.f, -4.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/rifleRunBack", owner->GetModel()));

	walk = 0.f;
}

void HunterAnimControl::Tick(float delta)
{
	AnimationController::Tick(delta);

	auto player = dynamic_cast<Hunter*>(owner->GetParent());
	if (player) {
		walk = Vector(Vector::Dot(player->GetRotation().GetRightVector(), player->move->DesiredState.velocity),
			Vector::Dot(player->GetRotation().GetForwardVector(), player->move->DesiredState.velocity),
			0.f);
	}
}

void HunterAnimControl::EvaluateBones(BoneArray bones)
{
	blender.Evaluate(last_delta, bones, walk.Y, walk.X);
}
