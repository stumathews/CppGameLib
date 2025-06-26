#pragma once
#include "BehavioralSequence.h"

namespace gamelib
{
	/**
	 * \brief Also known as a filter
	 */
	class ConditionalSequence : public BehavioralSequence
	{
	public:

		// The condition must be a successful behavior before other actions can run afterwards

		// children in sequence:
		// condition
		// action
		// action
		// action
		// condition
		// action

		
		void AddCondition(Behavior* condition)
		{
			// Use insert() if you store children in std::vector
			children.push_back(condition);
		}

		void AddAction(Behavior* action)
		{
			children.push_back(action);
		}
	};
}
