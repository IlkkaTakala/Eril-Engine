#pragma once
#include <Core.h>
#include <Skeleton.h>
#include <Animation.h>
#include <Interface/AssetManager.h>

class SkeletalObject;

class AnimationBlendSpace1D
{
	std::vector<std::pair<float, AnimationInstance>> anims;

public:

	void AddKey(float time, Animation* animation) {
		anims.emplace_back(time, animation);
	}

	void Evaluate(float delta, BoneArray bones, float axisValue) {

		const std::pair<float, AnimationInstance>* first = nullptr;
		const std::pair<float, AnimationInstance>* second = nullptr;

		if (anims.size() == 0) {
			for (auto& t : bones) t = Transform();
			return;
		}

		if (axisValue <= anims[0].first) {

			for (auto& a : anims)
				a.second.Update(delta, anims[0].second.GetFactor());

			anims[0].second.MakeTransforms(bones);
			return;
		}
		else if (axisValue >= anims.rbegin()->first) {

			for (auto& a : anims)
				a.second.Update(delta, anims.rbegin()->second.GetFactor());

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
					float p = first->second.GetFactor() * (1 - scale) + second->second.GetFactor() * scale;

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

class AnimationBlendSpace2D 
{
	std::vector<std::tuple<float, float, AnimationInstance>> anims;

	std::vector<Transform> old;
	std::vector<Transform> current;

	float interpTime;
	float currentTime;
	float interpolating;
	Vector values;

public:
	AnimationBlendSpace2D() {
		interpTime = 0.1f;
		currentTime = 0.f;
		interpolating = false;
	}

	void AddKey(float x, float y, Animation* animation) {
		anims.emplace_back(x, y, animation);
	}

	void Evaluate(float delta, BoneArray bones, float axisValueX, float axisValueY) {

		if (anims.size() == 0) {
			for (auto& t : bones) t = Transform();
			return;
		}

		old.resize(bones.size());
		current.resize(bones.size());

		std::tuple<float, float, const AnimationInstance*> instances[4] = {
			{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), nullptr },
			{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), nullptr },
			{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), nullptr },
			{ std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), nullptr },
		};

		int count = 0;
		for (const auto& [x, y, anim] : anims) {
			float deltax = x - axisValueX;
			float deltay = y - axisValueY;
			float distance = deltax * deltax + deltay * deltay;

			float det = -deltax;
			float dot = Vector::Dot({0, 1, 0}, Vector(deltax, deltay, 0.f).Normalize());
			float angle = atan2(det, dot);

			bool success = false;
			int counter = 0;
			for (const auto& [d, a, anima] : instances) {
				if (a != std::numeric_limits<float>::infinity() && (angle < a + 0.1f && angle > a - 0.1f)) {
					if (distance < d) {
						instances[counter] = { distance, angle, &anim };
						success = true;
						count++;
						break;
					}
				}
				counter++;
			}

			if (!success)
			for (int i = 0; i < 4; ++i) {
				float savedAngle = std::get<1>(instances[i]);
				
				if (distance < std::get<0>(instances[i])) {
					for (int j = 3; j > i; --j) {
						instances[j] = std::move(instances[j - 1]);
					}
					instances[i] = {distance, angle, &anim};
					count++;
					break;
				}
			}
		}
		count = 0;
		for (const auto& [d, a, anim] : instances) {
			if (anim) ++count;
		}

		float speed = 0.f;

		if (count > 1) {

			float max = 0.f;
			for (int i = 0; i < count; i++) {
				max += 1.f / std::get<0>(instances[i]);
			}

			for (int i = 0; i < count; i++) {
				std::get<0>(instances[i]) = (1.f / std::get<0>(instances[i])) / max;
				if (isnan(std::get<0>(instances[i]))) std::get<0>(instances[i]) = 1.f;
			}

			for (int i = 0; i < count; i++) {
				speed += std::get<2>(instances[i])->GetFactor() * std::get<0>(instances[i]);
			}
		}
		else {
			std::get<0>(instances[0]) = 1.f;
			speed = std::get<2>(instances[0])->GetFactor();
		}



		for (auto& [x, y, anim] : anims)
			anim.Update(delta, speed);

		float nextFactor = 1.f;
		float animFactor = delta / interpTime;
		Vector current(axisValueX, axisValueY, 0.f);
		float distance = (current - values).Length();
		if (distance > 0.01f) {
			values = current;
			interpolating = true;
			currentTime = 0.f;
		}
		if (interpolating) {
			currentTime += delta;
			if (currentTime > interpTime) {
				interpolating = false;
			}
			else 
				nextFactor = currentTime / interpTime;
		}

		for (int i = 0; i < bones.size(); i++) {
			for (const auto& [factor, dot, anim] : instances) {
				if (factor < 0.001 || anim == nullptr) continue;
				bones[i] = Transform::Interpolate(bones[i], anim->GetTransform(i), factor);
			}
			old[i] = bones[i] = Transform::Interpolate(old[i], bones[i], nextFactor);
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
	float factor;

public:
	void SetBlendFactor(float in) {
		factor = in;
	}

	void Init(const std::function<void(float, BoneArray)>& basef, const std::function<void(float, BoneArray)>& blendf, Skeleton* skeleton, const String& rootbone, float blendweight = 1.f) {
		base.resize(skeleton->BoneCount);
		blend.resize(skeleton->BoneCount);
		weightArray.resize(skeleton->BoneCount);
		getBase = basef;
		getBlend = blendf;
		factor = 0.f;

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
			bones[i] = Transform::Interpolate(base[i], blend[i], weightArray[i] * factor);
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
	AnimationInstance idle;
	AnimationInstance gunPose;
	AnimationPerBoneBlend perBoneGun;
	Vector walk;
	float gun;
	bool gunStatus;
	bool gunInterp;
	float gunTime;
};