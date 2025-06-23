#pragma once
#include "Composite.h"

namespace gamelib
{
	/**
	 * \brief Selects the first behavior that succeeds and continues to select it until it fails.
	 * Upon failure of current behavior, it attempts to selects the next behavior in list (ignores previous child behaviors).
	 * This does not recheck previous behaviors, which likely are higher priority if the occur earlier in the list of behaviors to select
	 */
	class LastSuccessfulChildSelector : public Composite
	{
	protected:

		virtual ~LastSuccessfulChildSelector() override = default;

		virtual void OnInitialize() override
		{
			// Select the highest priority behavior,i.e the first in the list
			currentChildBehavior = children.begin();
		}

		BehaviorResult Update() override
		{
			// Run/update the current child

			while(true)
			{
				const BehaviorResult childStatus = (*currentChildBehavior)->DoUpdate();
								
				if(childStatus != BehaviorResult::Failure)
				{
					// Return after running child successfully

					// If any child succeeds or is running, we do too
					return childStatus;
				}

				// Current child failed, now we'll move to the next child
				// and consider it as the current child

				// Continue search for fallback behavior until the last child
				if(++currentChildBehavior == children.end())
				{
					return BehaviorResult::Failure;
				}
			}
		}
		
		Behaviors::iterator currentChildBehavior; // the child selector keeps running its current child until it fails....
	};
}
