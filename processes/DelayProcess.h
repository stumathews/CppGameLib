#pragma once
#include <processes/Process.h>

namespace gamelib
{
	class DelayProcess final : public Process
	{
		unsigned long delayTargetMs;
		unsigned long delayedMs;
	public:
		explicit DelayProcess(const unsigned long delayMs) : Process(), delayTargetMs(delayMs), delayedMs(0) {}
	protected:
		void OnUpdate(const unsigned long deltaMs) override
		{
			delayedMs += deltaMs; 
			if (delayedMs >= delayTargetMs)
				Succeed();
		}
	};
}