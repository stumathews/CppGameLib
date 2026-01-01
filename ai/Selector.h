#pragma once
#include "Composite.h"

namespace gamelib
{
	// A selector composite runs each child behavior in order of priority until one succeeds
	class Selector : public Composite
	{
	protected:

		virtual ~Selector() override = default;

		virtual void OnInitialize() override
		{
			// Select the highest priority behavior,i.e. the first in the list
			child = children.begin();
		}

		Status Update(const unsigned long deltaMs) override
		{
			// Persistently keep running the last successful child
			while(child != children.end())
			{
				const auto childStatus = (*child)->Tick();

				// If any child succeeds or is running, we do too
				if(childStatus != Status::Failure)
				{					
					return childStatus;
				}

				++child;
			}

			// Unexpected loop exit
			return Status::Failure;
		}
		
		Behaviors::iterator child; // the child selector keeps running its current child until it fails....
	};
}
