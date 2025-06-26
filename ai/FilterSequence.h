#pragma once
#include "BehavioralSequence.h"

namespace gamelib
{
	// A filter is a branch in the tree that will not execute its child behavior under specific conditions
	class FilterSequence : public	BehavioralSequence
	{
	public:
		void AddCondition(Behavior* condition)
		{
			// Add a condition child that when it fails, causes the sequence to fail and therefore skip the next children
			children.push_back(condition);
		}

		void AddAction(Behavior* action)
		{
			children.push_back(action);
		}
	};
}
