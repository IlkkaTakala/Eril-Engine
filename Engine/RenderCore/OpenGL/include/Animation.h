#pragma once
#include <Core.h>

enum class InterpType : uint8 
{
	Smoothstep,
	Linear,
};

class Animation
{
	typedef std::tuple<Vector, InterpType> KeyframeLocation;
	typedef std::tuple<Vector, InterpType> KeyframeRotation;
	typedef std::tuple<Vector, InterpType> KeyframeScale;

	std::vector<std::list<std::pair<int, KeyframeLocation>>> LocationTrack;
	std::vector<std::list<std::pair<int, KeyframeRotation>>> RotationTrack;
	std::vector<std::list<std::pair<int, KeyframeScale>>> ScaleTrack;

	bool looping;
};

