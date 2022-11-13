#pragma once
#include <processes/Process.h>

namespace gamelib
{
	class DelayProcess: public Process
	{
		unsigned long delayTargetMs;
		unsigned long delayedMs;
	public:
		explicit DelayProcess(unsigned long delayMs) : delayTargetMs(delayMs), delayedMs(0) {}
	protected:
		virtual void OnUpdate(unsigned long deltaMs) override
		{
			delayedMs += deltaMs; 
			if (delayedMs >= delayTargetMs)
				Succeed();
		}
	};
}