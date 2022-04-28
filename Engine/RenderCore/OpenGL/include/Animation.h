#pragma once
#include <Core.h>

enum class InterpType : uint8 
{
	Smoothstep,
	Linear,
};

class Animation : public BaseObject
{
public:
	Vector GetLocation(int bone, float delta);
	Rotator GetRotation(int bone, float delta);
	Vector GetScale(int bone, float delta);

private:
	typedef std::tuple<Vector, InterpType> KeyframeLocation;
	typedef std::tuple<Rotator, InterpType> KeyframeRotation;
	typedef std::tuple<Vector, InterpType> KeyframeScale;

	std::vector<std::list<std::pair<float, KeyframeLocation>>> LocationTrack;
	std::vector<std::list<std::pair<float, KeyframeRotation>>> RotationTrack;
	std::vector<std::list<std::pair<float, KeyframeScale>>> ScaleTrack;

	bool looping;
};

