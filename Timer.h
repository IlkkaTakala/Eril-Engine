#pragma once
#include <map>
#include <functional>

class Timer
{
public:
	static void CreateTimer(float duration, std::function<void(void)> function, bool looping = false, bool runBeforeDuration = false);
	static void UpdateTimers(float delta);

	float GetElapsedTime();
private:
	Timer(float duration, std::function<void(void)> function, bool looping, bool runBeforeDuration);

	float delta;
	float duration;
	float totaltime;
	bool looping;
	bool shouldRun;
	bool complete;
	std::function<void(void)> callback;

	static std::list<Timer*> Timers;
};

