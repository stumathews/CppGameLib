#pragma once
#include "Status.h"
#include "Selector.h"

namespace gamelib
{
	// An active selector will always start from the highest priority child behavior on each update.
	class ActiveSelector : public Selector
	{
	protected:

		Status Update(const unsigned long deltaMs) override
		{
			const auto prevChildBehavior = child;

			// Rewind the current child selector back to the first child, effectively
			// allowing the decision-making sequence to start again from beginning thereby re-evaluating past decisions
			Selector::OnInitialize();

			// Update/Run last successful child or move to next child if it failed
			const auto result = Selector::Update(deltaMs);

			// Previous active child was not the end && current active child has changed 
			if(prevChildBehavior != children.end() && child != prevChildBehavior)
			{
				// abort failing child
				(*prevChildBehavior)->Abort();
			}

			return result;
		}
	};
}
