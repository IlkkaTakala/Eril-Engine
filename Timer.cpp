#include "Timer.h"

void Timer::CreateTimer(float duration, std::function<void(void)> function, bool looping, bool runBeforeDuration)
{
	Timers.push_back(new Timer(duration, function, looping, runBeforeDuration));
}

Timer::Timer(float duration, std::function<void(void)> function, bool looping, bool runBeforeDuration)
{
	this->duration = duration;
	this->callback = function;
	this->looping = looping;
	this->shouldRun = runBeforeDuration;
	this->complete = false;
}


void Timer::UpdateTimers(float delta)
{
	for (const auto& t : Timers) {
		t->delta += delta;
		t->totaltime += delta;
		if (t->delta > t->duration) {
			t->callback();
			t->delta = 0.f;

			if (!t->looping) t->complete = true;
		}
	}
	Timers.remove_if();
}

float Timer::GetElapsedTime()
{
	return totaltime;
}
