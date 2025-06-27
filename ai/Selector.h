#pragma once
#include "CompositeBehavior.h"

namespace gamelib
{
	/**
	 * \brief Selects the first behavior that succeeds and continues to select it until it fails.
	 * Upon failure of current behavior, it attempts to selects the next behavior in list (ignores previous child behaviors).
	 * This does not recheck previous behaviors, which likely are higher priority if the occur earlier in the list of behaviors to select
	 */
	class Selector : public CompositeBehavior
	{
	protected:

		virtual ~Selector() override = default;

		virtual void OnInitialize() override
		{
			// Select the highest priority behavior,i.e. the first in the list
			lastChild = children.begin();
		}

		BehaviorResult Update(const unsigned long deltaMs) override
		{
			// Persistently keep running the last child
			while(lastChild != children.end())
			{
				const auto childStatus = (*lastChild)->DoUpdate();
								
				if(childStatus != BehaviorResult::Failure)
				{
					// If any child succeeds or is running, we do too
					return childStatus;
				}

				// ok, last child failed, so move to the next child try to run it persistently also
				++lastChild;
			}

			// Unexpected loop exit
			return BehaviorResult::Failure;
		}
		
		Behaviors::iterator lastChild; // the child selector keeps running its current child until it fails....
	};
}
