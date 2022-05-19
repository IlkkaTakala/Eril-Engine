#pragma once
#include <Core.h>
#include <Objects/Actor.h>
#include <AnimationController.h>

class MovementComponent;
class SkeletalObject;

class HunterAnimControl : public AnimationController
{
public:
	HunterAnimControl(SkeletalObject* owner) : AnimationController(owner) {}

	void BeginPlay() override;
	void Tick(float delta) override;

	void EvaluateBones(BoneArray bones) override;

	AnimationBlendSpace2D blender;
	AnimationStateMachine states;
	AnimationInstance idle;
	AnimationInstance gunPose;
	AnimationPerBoneBlend perBoneGun;
	Vector walk;
	float gun;
	bool gunStatus;
	bool gunInterp;
	float gunTime;
};

class Hunter : public Actor
{
	REGISTER(Hunter)
public:
	Hunter();
	Ref<MovementComponent> move;
	bool gun;

	virtual void Tick(float) override;
private:
	Ref<SkeletalObject> Mesh;

	float time;
	Vector targetLoc;
	bool caught;

	void SetNewTarget(float);
};

