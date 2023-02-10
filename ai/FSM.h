#pragma once
#include "FSMState.h"
namespace gamelib
{
	// ReSharper disable once CppInconsistentNaming
	class FSM
	{
	public:
		void Update(unsigned long deltaMs);
		std::list<FSMState> States;
		FSMState* InitialState = nullptr;
		FSMState* ActiveState = nullptr;
	};
}

