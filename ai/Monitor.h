#pragma once
#include "Parallel.h"

namespace gamelib
{
	/**
	 * \brief Setup parallel conditions and actions that must all be succeeded at the same time to be successful a successful monitor
	 */
	class Monitor final : public Parallel
	{
	public:
		explicit Monitor(const Policy successPolicy = Policy::require_one, const Policy failurePolicy = Policy::require_one) :
		Parallel(successPolicy, failurePolicy) { }

		void AddCondition(Behavior* condition)
		{
			// Use insert() if you store children in std::vector
			children.insert(children.begin(), condition);
		}

		void AddAction(Behavior* action)
		{
			children.push_back(action);
		}
	};
}
