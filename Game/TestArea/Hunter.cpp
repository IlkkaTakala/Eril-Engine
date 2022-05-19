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
	Mesh->SetRotation(-180);
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
	Console::Log("New target set");
	if (GetGameState()->CurrentPlayer != nullptr)
		targetLoc = GetGameState()->CurrentPlayer->GetLocation();
}

void HunterAnimControl::BeginPlay()
{
	blender.AddKey(0.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Idle", owner->GetModel()));
	blender.AddKey(2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RightStrafeWalk", owner->GetModel()));
	blender.AddKey(-2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/LeftStrafeWalk", owner->GetModel()));
	blender.AddKey(0.f, 2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Walking", owner->GetModel()));
	blender.AddKey(0.f, -2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/WalkBack", owner->GetModel()));
	blender.AddKey(4.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RightStrafe", owner->GetModel()));
	blender.AddKey(-4.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/LeftStrafe", owner->GetModel()));
	blender.AddKey(0.f, 4.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Running", owner->GetModel()));
	blender.AddKey(0.f, -4.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RunBack", owner->GetModel()));

	walk = 0.f;
	gunStatus = false;
	gun = 0.f;
	gunTime = 0.3f;
	gunInterp = false;

	idle = AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Idle", owner->GetModel());
	gunPose = AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/GunPose", owner->GetModel());

	states.AddState("Walk", [&](float delta, BoneArray arr) {
		blender.Evaluate(delta, arr, walk.X, walk.Y);
	});

	auto skel = dynamic_cast<RenderMeshSkeletalGL*>(owner->GetModel())->GetSkeleton();
	perBoneGun.Init([&](float delta, BoneArray bones) {
		blender.Evaluate(delta, bones, walk.X, walk.Y);
	}, [&](float delta, BoneArray bones) {
		gunPose.MakeTransforms(bones);
	}, skel, "mixamorig:Spine");
	auto mesh = dynamic_cast<RenderMeshSkeletalGL*>(owner->GetModel());
}

void HunterAnimControl::Tick(float delta)
{
	AnimationController::Tick(delta);

	auto player = dynamic_cast<Hunter*>(owner->GetParent());
	if (player) {
		walk = Vector(Vector::Dot(player->GetRotation().GetRightVector(), player->move->DesiredState.velocity),
			Vector::Dot(player->GetRotation().GetForwardVector(), player->move->DesiredState.velocity),
			0.f);

		if (gunStatus != player->gun) {
			gunInterp = true;
			gunStatus = player->gun;
		}
	}
	if (gunInterp) {
		if (gunStatus) {
			gun += delta;
			if (gun > gunTime) {
				gun = gunTime;
				gunInterp = false;
			}
		}
		else {
			gun -= delta;
			if (gun < 0.f) {
				gun = 0.f;
				gunInterp = false;
			}
		}
	}
}

void HunterAnimControl::EvaluateBones(BoneArray bones)
{
	idle.Update(last_delta, idle.GetFactor());
	gunPose.Update(last_delta, gunPose.GetFactor());
	perBoneGun.SetBlendFactor(gun / gunTime);
	perBoneGun.Evaluate(last_delta, bones);
}
