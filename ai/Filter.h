#pragma once	

#include "Sequence.h"

namespace gamelib
{
	// A filter is a branch in the tree that will not execute its child behavior under specific conditions
	class Filter final : public gamelib::ai::Sequence
	{
	public:

		// A filter is a branch in the tree that will not execute its child behavior under specific conditions
		void AddCondition(gamelib::Behavior* condition)
		{
			// Add a condition child that when it fails, causes the sequence to fail and therefore skip the next children
			children.push_back(condition);
		}

		// Adds an action child behavior to this filter sequence
		void AddAction(gamelib::Behavior* action)
		{
			children.push_back(action);
		}
	};
}
