#pragma once
namespace gamelib
{
	class Timer
	{
	public:
		
		void Update(unsigned long deltaMs);

		void AddTime(unsigned long timeMs);

		void Start(long durationMs, long startTimeMs = 0);

		static long GetTimeMs();

		void Reset();

		void Stop();

		bool IsElapsed;
		bool IsStopped;

		long StartTimeMs;
		long ElapsedTimeMs;
		long TimerDurationMs;
	};
}

