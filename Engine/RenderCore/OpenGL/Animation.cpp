#include "Animation.h"

float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
	float scaleFactor = 0.0f;
	float midWayLength = animationTime - lastTimeStamp;
	float framesDiff = nextTimeStamp - lastTimeStamp;
	scaleFactor = midWayLength / framesDiff;
	return scaleFactor;
}

Animation::Animation() 
{
	loaded = false;
	looping = true;
	skeleton = nullptr;
	duration = 0.f;
	tickSpeed = 24;
}

Vector Animation::GetLocation(int bone, float delta)
{
	if (bone >= LocationTrack.size() || LocationTrack[bone].size() == 0) return 0.f;

	float a = delta * tickSpeed;
	float frametime = a - floor(a / duration) * duration;
	auto& frames = LocationTrack[bone];
	int begin = 0;
	for (int i = 0; i < frames.size(); i++) {
		if (frametime >= frames[i].first) {
			begin = i;
			break;
		}
	}

	int next = begin;
	if (next + 1 < frames.size()) ++next;
	float scaleFactor = GetScaleFactor(frames[begin].first,
		frames[next].first, delta);
	Vector finalPosition = frames[begin].second * (1 - scaleFactor) + frames[next].second;

	return finalPosition;
}

Rotator Animation::GetRotation(int bone, float delta)
{
	if (bone >= RotationTrack.size() || RotationTrack[bone].size() == 0) return Rotator(0.f);

	float a = delta * tickSpeed;
	float frametime = a - floor(a / duration) * duration;
	auto& frames = RotationTrack[bone];
	int begin = 0;
	for (int i = 0; i < frames.size(); i++) {
		if (frametime >= frames[i].first) {
			begin = i;
			break;
		}
	}

	int next = begin;
	if (next + 1 < frames.size()) ++next;
	float scaleFactor = GetScaleFactor(frames[begin].first,
		frames[next].first, delta);
	Rotator finalRotation = Rotator::Slerp(frames[begin].second, frames[next].second, scaleFactor).FastNormalize();

	return finalRotation;
}

Vector Animation::GetScale(int bone, float delta)
{
	if (bone >= ScaleTrack.size() || ScaleTrack[bone].size() == 0) return 1.f;

	float a = delta * tickSpeed;
	float frametime = a - floor(a / duration) * duration;
	auto& frames = ScaleTrack[bone];
	int begin = 0;
	for (int i = 0; i < frames.size(); i++) {
		if (frametime >= frames[i].first) {
			begin = i;
			break;
		}
	}

	int next = begin;
	if (next + 1 < frames.size()) ++next;
	float scaleFactor = GetScaleFactor(frames[begin].first,
		frames[next].first, begin * tickSpeed - delta);
	Vector finalScale = frames[begin].second * (1 - scaleFactor) + frames[next].second;

	return finalScale;
}
