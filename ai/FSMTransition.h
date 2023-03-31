#pragma once
#include <functional>
#include <utility>

namespace gamelib
{
	// ReSharper disable once CppInconsistentNaming
	class FSMState;
	// ReSharper disable once CppInconsistentNaming
	class FSMTransition
	{
	public:
		explicit FSMTransition(std::function<bool()> condition = Never, std::function<FSMState*()> getNextState = nullptr, std::function<void()> onTransition = DoNothing)
		: IsValid(std::move(condition)), GetNextState(std::move(getNextState)), OnTransition(std::move(onTransition)) {  }
		std::function<bool()> IsValid;
		std::function<FSMState*()> GetNextState;
		std::function<void()> OnTransition;
		static void DoNothing(){}
		static bool Never() {return false;}
	};
}
