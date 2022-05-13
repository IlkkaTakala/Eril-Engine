#include "AnimationController.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <SkeletalMesh.h>
#include <Objects/SkeletalObject.h>
#include "TestArea/TestPlayer.h"
#include "Gameplay/GameState.h"

AnimationController::AnimationController(SkeletalObject* owner) : owner(owner)
{
	temp_anim = nullptr;
	animtime = 0.f;
	last_delta = 0.f;
	animoverride = false;
}

void AnimationController::Tick(float delta)
{
	animtime += delta;
	last_delta = delta;
}

void AnimationController::SetSkeleton(RenderMesh* s)
{
}

void AnimationController::UpdateBoneTransforms(float delta, RenderMesh* mesh)
{
	if (!IsActive()) return;
	auto m = dynamic_cast<RenderMeshSkeletalGL*>(mesh);
	if (!m) return;

	auto& mats = m->GetBoneMatrices();

	if (animoverride && temp_anim) {
		for (int i = 1; i < mats.size(); i++) {
			Vector loc = temp_anim->GetLocation(i, animtime);
			Rotator rot = temp_anim->GetRotation(i, animtime);
			Vector sca = temp_anim->GetScale(i, animtime);

			mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
				* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
				* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
		}
	}
	else {
		if (boneTransforms.size() < mats.size()) boneTransforms.resize(mats.size());
		EvaluateBones(boneTransforms);
		for (int i = 1; i < mats.size(); i++) {
			Vector& loc = boneTransforms[i].Location;
			Rotator& rot = boneTransforms[i].Rotation;
			Vector& sca = boneTransforms[i].Scale;

			mats[i] = glm::translate(glm::mat4(1.0f), glm::vec3(loc.X, loc.Y, loc.Z))
				* glm::toMat4(glm::quat(rot.W, rot.X, rot.Y, rot.Z))
				* glm::scale(glm::mat4(1.0f), glm::vec3(sca.X, sca.Y, sca.Z));
		}
	}

	m->UpdateBoneMatrices();
}

void TestAnimControl::BeginPlay()
{
	blender.AddKey(0.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Idle", owner->GetModel()));
	blender.AddKey(5.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RightStrafeWalk", owner->GetModel()));
	blender.AddKey(-5.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/LeftStrafeWalk", owner->GetModel()));
	//blender.AddKey(2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RightStrafe", owner->GetModel()));
	//blender.AddKey(-2.f, 0.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/LeftStrafe", owner->GetModel()));
	blender.AddKey(0.f, 5.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Walking", owner->GetModel()));
	//blender.AddKey(0.f, 2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/Running", owner->GetModel()));
	blender.AddKey(0.f, -5.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/WalkBack", owner->GetModel()));
	//blender.AddKey(0.f, -2.f, AssetManager::LoadAnimationAsyncWithPromise("Assets/Animations/RunBack", owner->GetModel()));

	walk = 0.f;

	states.AddState("Walk", [&](float delta, BoneArray arr) {
		blender.Evaluate(delta, arr, walk.X, walk.Y);
	});
	auto mesh = dynamic_cast<RenderMeshSkeletalGL*>(owner->GetModel());
	

	//states.AddPaths("Walk", { {"Dance", [&]()->bool {return walk <= 0.f; }, 0.5f} });
	//states.AddPaths("Dance", {{"Walk", [&]()->bool {return walk > 0.f; }, 0.5f}});
}

void TestAnimControl::Tick(float delta)
{
	AnimationController::Tick(delta);

	auto player = dynamic_cast<TestPlayer*>(GetGameState()->CurrentPlayer.GetPointer());
	if (player) {
		walk = player->GetWalk();
	}
}

void TestAnimControl::EvaluateBones(BoneArray bones)
{
	//perBone.Evaluate(last_delta, bones);
	states.Evaluate(last_delta, bones);
}