#pragma once
#include <processes/Process.h>
#include <functional>

namespace gamelib
{
	class Action final : public Process
	{
	public:
		explicit Action(std::function<void()> action) : Process(), action(std::move(action)){}
		void OnUpdate(unsigned long deltaMs) override { action(); Succeed(); }
	private:
		std::function<void()> action;
	};
}