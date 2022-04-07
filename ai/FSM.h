#pragma once
#include "FSMState.h"
namespace gamelib
{
	class FSM
	{
	public:
		void Update();
		std::list<FSMState> States;
		FSMState* InitialState = nullptr;
		FSMState* ActiveState = nullptr;
	};
}

