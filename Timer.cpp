#include "Timer.h"

void gamelib::Timer::Update(float deltaMs)
{
	if(timerDurationMs == 0 || IsStopped) 
		return;

	elapsedTimeMs += deltaMs;
	IsElapsed = elapsedTimeMs > timerDurationMs ? true : false;
}

void gamelib::Timer::AddTime(float timeMs)
{
	elapsedTimeMs = (elapsedTimeMs - timeMs <= 0) ? 0 : elapsedTimeMs - timeMs;
}

void gamelib::Timer::Start(DWORD durationMs, DWORD startTimeMs)
{
	if (startTimeMs == 0)
	{
		Reset();
	}

	timerDurationMs = durationMs;

	IsStopped = IsElapsed = false;
	elapsedTimeMs = 0;
}


DWORD gamelib::Timer::GetTimeMs()
{
	return timeGetTime();
}

void gamelib::Timer::Reset()
{
	startTimeMs = GetTimeMs();
	elapsedTimeMs = 0;
	IsStopped = IsElapsed = false;
}

void gamelib::Timer::Stop()
{
	IsStopped = true;
}
