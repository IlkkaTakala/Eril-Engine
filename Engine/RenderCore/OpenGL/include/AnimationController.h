#pragma once
#include <Core.h>
#include <Skeleton.h>
#include <Animation.h>
#include <Interface/AssetManager.h>

class SkeletalObject;

struct AnimationBlendSpace1D
{
	std::vector<std::pair<float, AnimationInstance>> anims;

	void Evaluate(float delta, BoneArray bones, float axisValue) {

		const std::pair<float, AnimationInstance>* first = nullptr;
		const std::pair<float, AnimationInstance>* second = nullptr;

		if (anims.size() == 0) {
			for (auto& t : bones) t = Transform();
			return;
		}

		if (axisValue <= anims[0].first) {

			for (auto& a : anims)
				a.second.Update(delta, anims[0].second.anim->GetSpeedFactor());

			anims[0].second.MakeTransforms(bones);
			return;
		}
		else if (axisValue >= anims.rbegin()->first) {

			for (auto& a : anims)
				a.second.Update(delta, anims.rbegin()->second.anim->GetSpeedFactor());

			anims.rbegin()->second.MakeTransforms(bones);
			return;
		}
		else {
			for (int i = (int)anims.size() - 1; i >= 0; i--) {
				if (anims[i].first <= axisValue) {
					first = &anims[i];
					if (i + 1 < anims.size()) second = &anims[i + 1];
					break;
				}
			}

			if (first) {
				if (second) {
					float last = first->first;
					float next = second->first;
					float scale = (axisValue - last) / (next - last);
					float p = first->second.anim->GetSpeedFactor() * (1 - scale) + second->second.anim->GetSpeedFactor() * scale;

					for (auto& a : anims)
						a.second.Update(delta, p);

					for (int i = 0; i < bones.size(); i++) {

						Transform tempFirstTrans = first->second.GetTransform(i);
						Transform tempNextTrans = second->second.GetTransform(i);

						bones[i] = Transform::Interpolate(tempFirstTrans, tempNextTrans, scale);
					}
				}
				else {
					first->second.MakeTransforms(bones);
				}
			}
		}
	}
};

struct AnimationBlendSpace2D 
{
	std::vector<std::tuple<float, float, AnimationInstance>> anims;

	void Evaluate(float delta, BoneArray bones, float axisValueX, float axisValueY) {

		if (anims.size() == 0) {
			for (auto& t : bones) t = Transform();
			return;
		}

		// Find closest datapoint in each diagonal
		const AnimationInstance* first = nullptr;
		const AnimationInstance* second = nullptr;
		const AnimationInstance* third = nullptr;
		const AnimationInstance* fourth = nullptr;

		float upleft, downleft, upright, downright;
		upleft = downleft = upright = downright = std::numeric_limits<float>::infinity();

		for (const auto& [x, y, anim] : anims) {
			float deltaY = y - axisValueY;
			float deltaX = x - axisValueX;
			float distance = deltaX * deltaX + deltaY * deltaY;
			if (deltaX <= 0.f) {
				if (deltaY <= 0.f) {
					if (distance < upleft) {
						downleft = distance;
						fourth = &anim;
					}
				}
				else {
					if (distance < upleft) {
						upleft = distance;
						first = &anim;
					}
				}
			}
			else {
				if (deltaY <= 0.f) {
					if (distance < downright) {
						downright = distance;
						third = &anim;
					}
				}
				else {
					if (distance < upright) {
						upright = distance;
						second = &anim;
					}
				}
			}
		}

		int count = 4;
		if (!first) { upleft = 0.f; --count; }
		if (!second) { upright = 0.f; --count; }
		if (!third) { downright = 0.f; --count; }
		if (!fourth) { downleft = 0.f; --count; }

		float norm = 1.f / (upleft + downleft + upright + downright);
		upleft = upleft * norm;
		downleft = downleft * norm;
		upright = upright * norm;
		downright = downright * norm;

		if (count > 1) {
			upleft = 1.f - upleft;
			downleft = 1.f - downleft;
			upright = 1.f - upright;
			downright = 1.f - downright;
		}

		float speed = 0.f;

		if (first) speed += first->anim->GetSpeedFactor() * upleft;
		if (second) speed += second->anim->GetSpeedFactor() * upright;
		if (third) speed += third->anim->GetSpeedFactor() * downright;
		if (fourth) speed += fourth->anim->GetSpeedFactor() * downleft;

		for (auto& [x, y, anim] : anims)
			anim.Update(delta, speed);

		for (int i = 0; i < bones.size(); i++) {
			if (first && upleft > 0.f) bones[i] = Transform::Interpolate(bones[i], first->GetTransform(i), upleft);
			if (second && upright > 0.f) bones[i] = Transform::Interpolate(bones[i], second->GetTransform(i), upright);
			if (third && downright > 0.f) bones[i] = Transform::Interpolate(bones[i], third->GetTransform(i), downright);
			if (fourth && downleft > 0.f) bones[i] = Transform::Interpolate(bones[i], fourth->GetTransform(i), downleft);
		}
	}
};

class AnimationStateMachine
{
	typedef std::function<bool(void)> PathFunc;
public:
	AnimationStateMachine() {
		currentState = 0;
		interpolating = false;
		interpTime = 0.f;
		currentInterpTime = 0.f;
		maxState = -1;
	}

	void Evaluate(float delta, BoneArray bones) {

		if (states.empty()) return;
		if (current.size() != bones.size()) { current.resize(bones.size()); old.resize(bones.size()); }

		states[currentState](delta, current);

		if (interpolating) {
			currentInterpTime += delta;
			if (currentInterpTime > interpTime) { interpolating = false; }
			else {
				float d = currentInterpTime / interpTime;

				for (int i = 0; i < bones.size(); i++)
					bones[i] = Transform::Interpolate(old[i], current[i], d);
			}
		}
		else {
			for (int i = 0; i < bones.size(); i++)
				bones[i] = current[i];
		}

		if (paths.size() > currentState)
		for (const auto& [target, func, time] : paths[currentState]) {
			if (func()) {
				std::copy(bones.begin(), bones.end(), old.begin());
				currentState = target;
				interpolating = true;
				interpTime = time;
				currentInterpTime = 0.f;
				break;
			}
		}
	}

	void AddState(const String& name, std::function<void(float, BoneArray)> func) {
		if (ids.find(name) == ids.end()) {
			ids.emplace(name, ++maxState);

			states.emplace_back(func);
		}
	}

	void AddPaths(const String& name, const std::vector<std::tuple<String, std::function<bool(void)>, float>>& statepaths) {
		if (paths.size() != states.size()) paths.resize(states.size());
		if (ids.find(name) != ids.end()) {
			auto& it = paths[ids[name]];
			for (auto& [name, func, time] : statepaths)
				it.emplace_back(ids[name], func, time);
		}
	}

private:
	std::vector<Transform> old;
	std::vector<Transform> current;

	std::unordered_map<String, int> ids;

	std::vector<std::function<void(float, BoneArray)>> states;
	std::vector<std::vector<std::tuple<int, std::function<bool(void)>, float>>> paths;

	int maxState;

	int currentState;
	bool interpolating;
	float interpTime;
	float currentInterpTime;
};

class AnimationPerBoneBlend
{
	std::function<void(float, BoneArray)> getBase;
	std::function<void(float, BoneArray)> getBlend;
	std::vector<float> weightArray;
	std::vector<Transform> base;
	std::vector<Transform> blend;

public:
	void Init(const std::function<void(float, BoneArray)>& basef, const std::function<void(float, BoneArray)>& blendf, Skeleton* skeleton, const String& rootbone, float blendweight = 1.f) {
		base.resize(skeleton->BoneCount);
		blend.resize(skeleton->BoneCount);
		weightArray.resize(skeleton->BoneCount);
		getBase = basef;
		getBlend = blendf;

		int parent = -1;

		for (uint i = 0; i < skeleton->BoneCount; ++i) {
			Bone& b = skeleton->Bones[i];
			if (parent == -1) {
				if (b.name == rootbone) {
					parent = b.id;
					weightArray[i] = blendweight;
				}
			}
			else {
				bool isParent = false;
				int idx = i;
				while (skeleton->Bones[idx].parent != -1) {
					if (skeleton->Bones[idx].parent == parent) {
						isParent = true;
						break;
					}
					idx = skeleton->Bones[idx].parent;
				}
				if (isParent) {
					weightArray[i] = blendweight;
				}
			}
		}
	}

	void Evaluate(float delta, BoneArray bones) {
		getBase(delta, base);
		getBlend(delta, blend);

		for (int i = 0; i < bones.size(); ++i) {
			bones[i] = Transform::Interpolate(base[i], blend[i], weightArray[i]);
		}
	}
};

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
	virtual void EvaluateBones(BoneArray) = 0;

	Animation* temp_anim;
	float animtime;
	float last_delta;
	std::vector<Transform> boneTransforms;

	bool animoverride;

	SkeletalObject* owner;
};

class TestAnimControl : public AnimationController
{
public:
	TestAnimControl(SkeletalObject* owner) : AnimationController(owner) {}

	void BeginPlay() override;
	void Tick(float delta) override;

	void EvaluateBones(BoneArray bones) override;

	AnimationBlendSpace2D blender;
	AnimationStateMachine states;
	AnimationPerBoneBlend perBone;
	AnimationInstance dance;
	float walk;
};