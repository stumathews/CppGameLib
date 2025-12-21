#pragma once
#include <functional>
#include <time/time.h>

namespace gamelib
{
	class Profiler
	{
	public:
		static long GetTimeNowMs()
		{
			return static_cast<long>(GetTimeMs());
		}
		static long GetDuration(const std::function<void()> funcToProfile)
		{
			const auto before = GetTimeNowMs();
			funcToProfile();
			const auto after = GetTimeNowMs();
			return after - before;
		}
	};
}

