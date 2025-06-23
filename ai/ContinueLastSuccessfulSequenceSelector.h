#pragma once
#include "BehaviorResult.h"
#include "Composite.h"
#include "LastSuccessfulChildSelector.h"

namespace gamelib
{
	class ContinueLastSuccessfulSequenceSelector : public LastSuccessfulChildSelector
	{
	protected:
		BehaviorResult Update() override
		{
			const auto prevChildBehavior = currentChildBehavior;

			// Rewind the current child selector back to the first child, effectively
			// allowing the sequence to start again
			LastSuccessfulChildSelector::OnInitialize(); // reset to select the first (hightest priority) behavior in list of child behaviors

			// Update/Run last successful child or move to next child if it failed (Skipping failed behaviors)
			const auto result = LastSuccessfulChildSelector::Update();

			// previous active child was not the end && current active child has changed 
			if(prevChildBehavior != children.end() && currentChildBehavior != prevChildBehavior)
			{
				// abort failing child
				(*prevChildBehavior)->Abort();
			}

			return result;
		}
	};
}
