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

		void Restart();

		void Stop();

		bool IsElapsed;
		bool IsStopped;

		unsigned long StartTimeMs;
		unsigned long ElapsedTimeMs;
		unsigned long TimerDurationMs;
	};
}

