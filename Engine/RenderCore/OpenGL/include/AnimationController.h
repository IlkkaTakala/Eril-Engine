#pragma once
#include <Core.h>
#include <Skeleton.h>
#include <Animation.h>

struct LoadedAnimation
{
	Animation* anim;
	float localTime;
};

struct AnimationBlendSpace
{

};

struct AnimationCombiner 
{
	virtual void Initialize() = 0;
	virtual void Evaluate(float delta, int bone, Vector& location, Rotator& rotation, Vector& scale) = 0;
};

class AnimationStateMachine
{
	typedef std::function<bool(void)> PathFunc;
public:
	AnimationStateMachine(const std::unordered_map<String, AnimationCombiner*>& combiners, const std::multimap<String, std::pair<String, PathFunc>>& paths);

	AnimationCombiner* Evaluate();

private:
	std::vector<AnimationCombiner*> Coms;
	std::multimap<int, std::pair<int, PathFunc>> Paths;

	int currentState;
};

#define MAKE_SM_STATE(NAME, ANIMCOMBINER) {#NAME, ANIMCOMBINER}
#define MAKE_SM_PATH(NAME, NAME2, FUNC) {#NAME, {#NAME2, FUNC}}

struct TestCombiner : public AnimationCombiner
{
	void Evaluate(float delta, int bone, Vector& location, Rotator& rotation, Vector& scale) {
		switch (state)
		{
		case 0:
			test();
			test2();
			return animationcombiner.evaluate();
		default:
			break;
		}
	}

	Animation* base;
};

class AnimationController : public BaseObject, public Tickable
{
public:
	AnimationController();

	void BeginPlay() override {}
	void Tick(float delta) override;
	void SetOverrideAnimation(Animation* anim) { temp_anim = anim; animoverride = true; }
	void SetCombiner(AnimationCombiner* anim) { combiner = anim; }
	void SetSkeleton(Skeleton* s) { }
	void SetSkeleton(RenderMesh* s);

	void UpdateBoneTransforms(float delta, RenderMesh* mesh);

private:
	AnimationCombiner* combiner;
	Animation* temp_anim;
	float animtime;

	bool animoverride;
};

