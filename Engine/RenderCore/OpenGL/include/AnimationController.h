#pragma once
#include <Core.h>
#include <Skeleton.h>
#include <Animation.h>
#include <Interface/AssetManager.h>

class SkeletalObject;

struct LoadedAnimation
{
	Animation* anim;
	float localTime;
};

struct AnimationBlendSpace1D
{
	std::vector<std::pair<float, Animation*>> anims;

	float min;
	float max;

	Transform Evaluate(float delta, int bone, float axisValue) const {
		const std::pair<float, Animation*>* first = nullptr;
		const std::pair<float, Animation*>* second = nullptr;

		if (anims.size() == 0) return Transform();

		if (axisValue <= anims[0].first) return anims[0].second->GetTransform(bone, delta);
		if (axisValue >= anims.rbegin()->first) return anims.rbegin()->second->GetTransform(bone, delta);

		for (int i = (int)anims.size() - 1; i >= 0; i--) {
			if (anims[i].first <= axisValue) {
				first = &anims[i];
				if (i + 1 < anims.size()) second = &anims[i + 1];
				break;
			}
		}
		Transform finalTrans;

		if (first) {
			if (second) {
				float last = first->first;
				float next = second->first;
				float scale = (axisValue - last) / (next - last);

				Transform tempFirstTrans;
				Transform tempNextTrans;
				//tempFirstTrans = first->second->GetTransform(bone, delta);
				float p = first->second->GetPercentageFromDuration(delta) * (1 - scale) + second->second->GetPercentageFromDuration(delta) * scale;
				tempNextTrans = second->second->GetTransformByPercentage(bone, p);
				tempFirstTrans = first->second->GetTransformByPercentage(bone, p);

				//tempNextTrans = second->second->GetTransform(bone, delta);
				finalTrans.Location = tempFirstTrans.Location * (1 - scale) + tempNextTrans.Location * scale;
				finalTrans.Rotation = Rotator::Slerp(tempFirstTrans.Rotation, tempNextTrans.Rotation, scale).FastNormalize();
				finalTrans.Scale = tempFirstTrans.Scale * (1 - scale) + tempNextTrans.Scale * scale;
			}
			else {
				finalTrans = first->second->GetTransform(bone, delta);
			}
		}

		return finalTrans;
	}
};

struct AnimationBlendSpace2D 
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

class AnimationController : public BaseObject, public Tickable
{
public:
	AnimationController(SkeletalObject* owner);

	virtual void BeginPlay() override {}
	virtual void Tick(float delta) override;
	void SetOverrideAnimation(Animation* anim) { temp_anim = anim; animoverride = true; }
	void SetSkeleton(Skeleton* s) { }
	void SetSkeleton(RenderMesh* s);

	void UpdateBoneTransforms(float delta, RenderMesh* mesh);

protected:
	virtual Transform EvaluateBone(int bone) const = 0;

	Animation* temp_anim;
	float animtime;

	bool animoverride;

	SkeletalObject* owner;
};

class TestAnimControl : public AnimationController
{
public:
	TestAnimControl(SkeletalObject* owner) : AnimationController(owner) {}

	void BeginPlay() override;

	Transform EvaluateBone(int bone) const override {
		return blender.Evaluate(animtime, bone, 0.51f);
	}

	AnimationBlendSpace1D blender;
};