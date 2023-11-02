#pragma once
#include <windows.h>
#include <functional>

namespace gamelib
{
	class Profiler
	{
	public:
		static long GetTimeNowMs()
		{
			return static_cast<long>(timeGetTime());
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

