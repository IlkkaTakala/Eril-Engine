#pragma once
#include <map>
#include <functional>

class Timer
{
public:
	static void CreateTimer(float duration, std::function<void(float)> function, bool looping = false, bool runBeforeDuration = false);
	static void UpdateTimers(float delta);

	bool IsComplete() const { return complete; }
	float GetElapsedTime();
private:
	Timer(float duration, std::function<void(float)> function, bool looping, bool runBeforeDuration);

	float delta;
	float duration;
	float totaltime;
	bool looping;
	bool shouldRun;
	bool complete;
	std::function<void(float)> callback;

	static std::list<Timer*> Timers;
};

