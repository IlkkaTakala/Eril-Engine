#pragma once
#include <Core.h>

class Skeleton;
class Animation;

typedef std::vector<Transform>& BoneArray;

enum class InterpType : uint8 
{
	Smoothstep,
	Linear,
};

class Animation
{
public:
	Animation();

	Vector GetLocation(int bone, float delta) const;
	Rotator GetRotation(int bone, float delta) const;
	Vector GetScale(int bone, float delta) const;
	Transform GetTransform(int bone, float delta) const;
	Transform GetTransformByPercentage(int bone, float percent) const;

	float GetDuration() const;
	float GetSpeedFactor() const;
	float GetPercentageFromDuration(float time) const;

	bool IsReady() const { return loaded; }

	typedef std::tuple<Vector, InterpType> KeyframeLocation;
	typedef std::tuple<Rotator, InterpType> KeyframeRotation;
	typedef std::tuple<Vector, InterpType> KeyframeScale;

	std::vector<std::vector<std::pair<float, Vector>>> LocationTrack;
	std::vector<std::vector<std::pair<float, Rotator>>> RotationTrack;
	std::vector<std::vector<std::pair<float, Vector>>> ScaleTrack;

	bool loaded;
	bool looping;
	float duration;
	int tickSpeed;
	float durationSeconds;
	float speedFactor;

	Skeleton* skeleton;
};

struct AnimationInstance
{
private:
	Animation* anim{nullptr};
	float frametime{0};

public:
	AnimationInstance() : anim(nullptr) {}
	AnimationInstance(Animation* a) : anim(a) {}

	void SetAnimation(Animation* a) {
		anim = a;
		frametime = 0.f;
	}

	float GetFactor() const {
		return anim->GetSpeedFactor();
	}

	void Update(float delta, float factor) {
		frametime += delta * factor;
		if (frametime > 1.f) frametime -= floor(frametime);
		else if (frametime < 0.f) frametime -= floor(1.f);
	}

	void MakeTransforms(BoneArray bones) const {
		for (int i = 0; i < bones.size(); i++) bones[i] = anim->GetTransform(i, frametime);
	}

	Transform GetTransform(int bone) const {
		return anim->GetTransform(bone, frametime);
	}
};

