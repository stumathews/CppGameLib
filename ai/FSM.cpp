#include "FSM.h"
#include <exceptions/EngineException.h>

namespace gamelib
{
	void FSM::Update(unsigned long deltaMs)
	{
		if (ActiveState == nullptr)
		{
			ActiveState = InitialState;			
		}

		if (InitialState == nullptr)
		{
			THROW(12, "No valid initial state could be determined", "Finite State Machine");
		}

		// Look if we need to transition from active/current state
		for (auto& [IsValid, GetNextState, OnTransition] : ActiveState->Transitions)
		{
			// Look for a condition that means we need to transition
			if (IsValid())
			{
				// Finish up on current state
				ActiveState->OnExit();

				// Indicate that this transition is transitioning
				OnTransition();

				// Set the next active state to the transitioning state
				ActiveState = GetNextState();		

				// Enter the newly transitioned to state i.e the new active state
				ActiveState->OnEnter();
				
				break;
			}
		}

		// Do some more stuff in the current/active state
		ActiveState->OnUpdate(deltaMs);
	}
}
