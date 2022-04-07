#pragma once
#include <list>
#include <functional>
#include "FSMTransition.h"

namespace gamelib
{
	class FSMState
	{
	public:
		FSMState()
		{
			OnEnter = []() {};
			OnExit= []() {};
			OnUpdate = []() {};
		}
		std::function<void()> OnEnter;
		std::function<void()> OnUpdate;
		std::function<void()> OnExit;
		std::list<FSMTransition> Transitions;
	};
}

