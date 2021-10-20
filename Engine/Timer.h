#pragma once
#include <map>
#include <functional>

class Timer
{
public:
	static void CreateTimer(float duration, void (*Callback)(float), bool looping = false, bool runBeforeDuration = false);
	
	template <class UserClass>
	static void CreateTimer(float duration, void (UserClass::* Callback)(float), UserClass* Caller, bool looping = false, bool runBeforeDuration = false)
	{
		using std::placeholders::_1;
		std::function<void(float)> f = std::bind(Callback, Caller, _1);
		Timers.push_back(new Timer(duration, f, looping, runBeforeDuration));
	}

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

