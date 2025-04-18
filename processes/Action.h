#pragma once
#include <processes/Process.h>
#include <functional>

namespace gamelib
{
	class Action final : public Process
	{
	public:
		explicit Action(std::function<void(unsigned long deltaMs)> action, const bool succeedAfterExecution = true) : Process(), action(std::move(action)), succeedAfterExecution(succeedAfterExecution) {}
		void OnUpdate(const unsigned long deltaMs) override
		{
			action(deltaMs);

			// By default, this action/process will be removed from the process manager once its run, unless....
			if(succeedAfterExecution)
			{
				Succeed();
			}
		}
	private:
		std::function<void(unsigned long)> action;
		bool succeedAfterExecution;
	};
}