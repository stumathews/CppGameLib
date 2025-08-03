#pragma once
#include "BehaviorResult.h"
#include "Selector.h"

namespace gamelib
{
	class ActiveSelector : public Selector
	{
	protected:
		BehaviorResult Update(const unsigned long deltaMs) override
		{
			const auto prevChildBehavior = lastChild;

			// Rewind the current child selector back to the first child, effectively
			// allowing the decision-making sequence to start again from beginning
			Selector::OnInitialize(); // reset to select the first (hightest priority) behavior in list of child behaviors

			// Update/Run last successful child or move to next child if it failed (Skipping failed behaviors)
			const auto result = Selector::Update(deltaMs);

			// previous active child was not the end && current active child has changed 
			if(prevChildBehavior != children.end() && lastChild != prevChildBehavior)
			{
				// abort failing child
				(*prevChildBehavior)->Abort();
			}

			return result;
		}
	};
}
