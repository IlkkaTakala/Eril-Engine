#pragma once
#include <Core.h>
#include <Skeleton.h>
#include <Animation.h>

class AnimationController : public BaseObject, public Tickable
{
public:
	AnimationController();

	void BeginPlay() override {}
	void Tick(float delta) override;
	void SetAnimation(Animation* anim) { temp_anim = anim; }
	void SetSkeleton(Skeleton* s) { }
	void SetSkeleton(RenderMesh* s);

	void UpdateBoneTransforms(float delta, RenderMesh* mesh);

private:
	Animation* temp_anim;
	float animtime;
};

