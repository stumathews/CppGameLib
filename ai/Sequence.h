#pragma once
#include "BehaviorResult.h"
#include "Composite.h"

namespace gamelib
{
	class Sequence : public Composite
	{
		protected:
		~Sequence() override
		{
	    }
		virtual void OnInitialize() override
		{
			currentChild = children.begin();
		}

		BehaviorResult Update() override
		{
			// Tick each child behavior in the sequence
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
