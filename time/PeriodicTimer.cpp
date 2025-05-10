#include "PeriodicTimer.h"

void gamelib::PeriodicTimer::DoIfReady(const std::function<void()>& action)
{
	if(ready)
	{
		action();
		Reset();
	}
}

void gamelib::PeriodicTimer::Update(const unsigned long deltaMs)
{
	if(timer.TimerDurationMs == 0)
	{
		timer.Start(everyMs);
	}

	timer.Update(deltaMs);

	ready = timer.IsElapsed;
}

void gamelib::PeriodicTimer::SetFrequency(const int everyMs)
{
	this->everyMs = everyMs;
}

void gamelib::PeriodicTimer::Reset()
{
	timer.Reset();
	timer.Start(everyMs);
}
