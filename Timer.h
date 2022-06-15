#pragma once
#include <windows.h>
namespace gamelib
{
	class Timer
	{
	public:
		
		void Update(float deltaMs);

		void AddTime(float timeMs);

		void Start(DWORD durationMs, DWORD startTimeMs = 0);

		DWORD GetTimeMs();

		void Reset();

		void Stop();

		bool IsElapsed;
		bool IsStopped;

		DWORD startTimeMs;
		DWORD elapsedTimeMs;
		DWORD timerDurationMs;
	};
}

