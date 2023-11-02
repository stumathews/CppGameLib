#include "PeriodicTimer.h"
#include <exceptions/EngineException.h>

void gamelib::PeriodicTimer::DoIfReady(const std::function<void()>& action)
{
	if(ready)
	{
		action();
		timer.Reset();
		timer.Start(everyMs);
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

void gamelib::PeriodicTimer::SetFrequency(int everyMs)
{
	this->everyMs = everyMs;
}
