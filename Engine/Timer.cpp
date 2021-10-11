#include "Timer.h"

std::list<Timer*> Timer::Timers;

void Timer::CreateTimer(float duration, std::function<void(float)> function, bool looping, bool runBeforeDuration)
{
	Timers.push_back(new Timer(duration, function, looping, runBeforeDuration));
}

Timer::Timer(float duration, std::function<void(float)> function, bool looping, bool runBeforeDuration)
{
	this->delta = 0.f;
	this->totaltime = 0.f;
	this->duration = duration;
	this->callback = function;
	this->looping = looping;
	this->shouldRun = runBeforeDuration;
	this->complete = false;
}


void Timer::UpdateTimers(float delta)
{
	bool completeOnIterator = false;
	for (const auto& t : Timers) {
		t->delta += delta;
		t->totaltime += delta;
		if (t->delta > t->duration) {
			t->callback(t->delta);
			t->delta = t->delta - t->duration;

			if (!t->looping) {
				t->complete = true;
				completeOnIterator = true;
			}
		}
	}
	if (completeOnIterator) {
		auto pend = std::remove_if(Timers.begin(), Timers.end(), [](Timer* t) { return t->IsComplete(); });
		Timers.erase(pend, Timers.end());
	}
}

float Timer::GetElapsedTime()
{
	return totaltime;
}
