#pragma once
#include <windows.h>
namespace gamelib
{
	class Timer
	{
	public:
		
		void Update(unsigned long deltaMs);

		void AddTime(unsigned long timeMs);

		void Start(DWORD durationMs, DWORD startTimeMs = 0);

		static DWORD GetTimeMs();

		void Reset();

		void Stop();

		bool IsElapsed;
		bool IsStopped;

		DWORD StartTimeMs;
		DWORD ElapsedTimeMs;
		DWORD TimerDurationMs;
	};
}

