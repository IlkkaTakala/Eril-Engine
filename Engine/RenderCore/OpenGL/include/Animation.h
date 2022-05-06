#pragma once
#include <Core.h>

class Skeleton;

enum class InterpType : uint8 
{
	Smoothstep,
	Linear,
};

class Animation
{
public:
	Animation();

	Vector GetLocation(int bone, float delta, float speed = 1.f) const;
	Rotator GetRotation(int bone, float delta, float speed = 1.f) const;
	Vector GetScale(int bone, float delta, float speed = 1.f) const;
	Transform GetTransform(int bone, float delta, float speed = 1.f) const;
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

