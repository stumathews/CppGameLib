#pragma once
#include "BehaviorResult.h"
#include "Composite.h"

namespace gamelib
{
	// A sequence is a composite behavior (stores multiple behaviors) of activities that are all run consecutively.
	// If one activity/behavior fails the whole sequence short circuits and fails
	class ShortCircuitSequence : public Composite
	{
		protected:
		~ShortCircuitSequence() override = default;

		virtual void OnInitialize() override
		{
			// Initialise sequence to start at the first behavior in the sequence
			currentChild = children.begin();
		}

		BehaviorResult Update() override
		{
			// Keep running the current child, until it's update returns failure
			while(true)
			{
				// Run the child behavior
				const BehaviorResult childStatus = (*currentChild)->DoUpdate();

				// Check what happened 
				if(childStatus != BehaviorResult::Success)
				{
					// If any child fails, we fail too
					return childStatus;
				}

				++currentChild;

				// Move to the next child and if we're at the last child, finish loop and return success 
				if(currentChild == children.end())
				{
					return BehaviorResult::Success;
				}
			}

			// Unexpected loop exit
			return BehaviorResult::Invalid;
		}

		Behaviors::iterator currentChild;
	};
}
