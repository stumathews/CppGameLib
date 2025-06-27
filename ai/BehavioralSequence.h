#pragma once
#include "BehaviorResult.h"
#include "CompositeBehavior.h"

namespace gamelib
{
	// A sequence is a composite behavior (stores multiple behaviors) of activities that are all run consecutively.
	// If one activity/behavior fails the whole sequence short circuits and fails
	class BehavioralSequence : public CompositeBehavior  // NOLINT(cppcoreguidelines-special-member-functions)
	{
		protected:
		~BehavioralSequence() override = default;

		virtual void OnInitialize() override
		{
			// Initialise sequence to start at the first behavior in the sequence
			currentChild = children.begin();
		}

		BehaviorResult Update(const unsigned long deltaMs) override
		{
			// Update each child
			while(currentChild != children.end())
			{
				// Run the child behavior
				const BehaviorResult childStatus = (*currentChild)->DoUpdate();

				// Check what happened 
				if(childStatus != BehaviorResult::Success)
				{
					// If any child fails, we fail too
					return childStatus;
				}

				// Move to the next child to run...
				++currentChild;
			}

			// All children were updated
			return BehaviorResult::Success;
		}

		Behaviors::iterator currentChild;
	};
}
