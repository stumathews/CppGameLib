#pragma once
#include "BehaviorResult.h"
#include "Composite.h"

namespace gamelib
{
	// A sequence of activities that are all run consecutively.
	// If one activity/behavior fails the whole sequence short circuits and fails
	class ShortCircuitingSequence : public Composite
	{
		protected:
		~ShortCircuitingSequence() override = default;

		virtual void OnInitialize() override
		{
			currentChild = children.begin();
		}

		BehaviorResult Update() override
		{
			// Run/Update all child behavior in the sequence
			while(true)
			{
				const BehaviorResult childStatus = (*currentChild)->DoUpdate();
				
				if(childStatus != BehaviorResult::Success)
				{
					// If any child fails, we fail too
					return childStatus;
				}
				
				if(++currentChild == children.end())
				{
					return BehaviorResult::Success;
				}
			}
		}

		Behaviors::iterator currentChild;
	};
}
