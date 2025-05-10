#pragma once
#include <functional>

#include "Timer.h"
namespace gamelib
{
	class PeriodicTimer
	{
	public:
		void DoIfReady(const std::function<void()>& action);
		void Update(unsigned long deltaMs);
		void SetFrequency(int everyMs);
		bool IsReady() const { return ready; }
		void Reset();
	private:
		Timer timer{};
		unsigned everyMs {};
		bool ready {};
	};
}

