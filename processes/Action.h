#pragma once
#include <processes/Process.h>
#include <functional>

namespace gamelib
{
	class Action : public Process
	{
	public:
		Action(std::function<void()> action) : action(action) {};
		virtual void OnUpdate(unsigned long deltaMs) override { action(); Succeed(); }
	private:
		std::function<void()> action;
	};
}