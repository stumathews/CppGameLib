#include "Timer.h"
#include <Windows.h>


void gamelib::Timer::Update(const unsigned long deltaMs)
{
	if(TimerDurationMs == 0 || IsStopped) 
		return;

	ElapsedTimeMs += deltaMs;
	IsElapsed = ElapsedTimeMs >= TimerDurationMs ? true : false;
}

void gamelib::Timer::AddTime(const unsigned long timeMs)
{
	ElapsedTimeMs = (ElapsedTimeMs - timeMs <= 0) ? 0 : ElapsedTimeMs - timeMs;
}

void gamelib::Timer::Start(const long durationMs, const long startTimeMs)
{
	if (startTimeMs == 0)
	{
		Reset();
	}

	TimerDurationMs = durationMs;

	IsStopped = IsElapsed = false;
	ElapsedTimeMs = 0;
}


long gamelib::Timer::GetTimeMs()
{
	return timeGetTime();
}

void gamelib::Timer::Reset()
{
	StartTimeMs = GetTimeMs();
	ElapsedTimeMs = 0;
	IsStopped = IsElapsed = false;
}

void gamelib::Timer::Stop()
{
	IsStopped = true;
}
