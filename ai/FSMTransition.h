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
		explicit FSMTransition(std::function<bool()> isValid = nullptr, std::function<FSMState*()> getNextState = nullptr, std::function<void()> onTransition = nullptr)
		: IsValid(std::move(isValid)), GetNextState(std::move(getNextState)), OnTransition(std::move(onTransition)) {  }
		std::function<bool()> IsValid;
		std::function<FSMState*()> GetNextState;
		std::function<void()> OnTransition;
	};
}
