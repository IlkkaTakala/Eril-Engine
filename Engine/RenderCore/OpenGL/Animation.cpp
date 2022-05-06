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

template <typename T>
inline bool GetFirst(const T& list, float duration, float speed, float delta, int& begin, int& next, float& scale)
{
	float clamped = delta - floor(delta / duration) * duration;
	float a = clamped * speed;
	float frametime = a * 30;
	begin = 0;
	for (int i = (int)list.size() - 1; i >= 0; i--) {
		if (frametime >= list[i].first) {
			begin = i;
			break;
		}
	}

	next = begin;
	if (next + 1 < list.size()) ++next;
	scale = GetScaleFactor(list[begin].first,
		list[next].first, frametime);

	return true;
}

Vector Animation::GetLocation(int bone, float delta, float speed) const
{
	if (bone >= LocationTrack.size() || LocationTrack[bone].size() == 0) return 0.f;

	auto& frames = LocationTrack[bone];
	int begin = 0;
	int next = 0;
	float scaleFactor = 0.f;
	GetFirst(frames, normDuration, speed, delta, begin, next, scaleFactor);


	Vector finalPosition = frames[begin].second * (1 - scaleFactor) + frames[next].second * scaleFactor;
	
	return finalPosition;
}

Rotator Animation::GetRotation(int bone, float delta, float speed) const
{
	if (bone >= RotationTrack.size() || RotationTrack[bone].size() == 0) return Rotator(0.f);

	auto& frames = RotationTrack[bone];
	int begin = 0;
	int next = 0;
	float scaleFactor = 0.f;
	GetFirst(frames, normDuration, speed, delta, begin, next, scaleFactor);

	Rotator finalRotation = Rotator::Slerp(frames[begin].second, frames[next].second, scaleFactor).FastNormalize();

	return finalRotation;
}

Vector Animation::GetScale(int bone, float delta, float speed) const
{
	if (bone >= ScaleTrack.size() || ScaleTrack[bone].size() == 0) return 1.f;

	
	auto& frames = ScaleTrack[bone];
	int begin = 0;
	int next = 0;
	float scaleFactor = 0.f;
	GetFirst(frames, normDuration, speed, delta, begin, next, scaleFactor);

	Vector finalScale = frames[begin].second * (1 - scaleFactor) + frames[next].second * scaleFactor;

	return finalScale;
}

Transform Animation::GetTransform(int bone, float delta, float speed) const
{
	return { GetLocation(bone, delta, speed), GetRotation(bone, delta, speed), GetScale(bone, delta, speed) };
}

Transform Animation::GetTransformByPercentage(int bone, float percent) const
{
	return GetTransform(bone, durationSeconds * percent);
}

float Animation::GetDuration() const
{
	return durationSeconds;
}

float Animation::GetSpeedFactor() const
{
	return normDuration;
}

float Animation::GetPercentageFromDuration(float time) const
{
	float a = time * tickSpeed;
	float frametime = a - floor(a / duration) * duration;
	return frametime / tickSpeed;
}
