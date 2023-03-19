#pragma once
#include "Composite.h"

namespace gamelib
{
	class Parallel : public Composite
	{
	public:
		enum Policy
		{
			RequireOne,
			RequireAll
		};
		Parallel(const Policy success, const Policy failure): successPolicy(success), failurePolicy(failure){}
		~Parallel() override = default;

	protected:
		Policy successPolicy;
		Policy failurePolicy;
		virtual BehaviorResult Update() override
		{
			size_t successCount = 0, failureCount = 0;

			// Run each child behavior in the same frame
			for(const auto childBehavior : children)
			{
				if(!childBehavior->IsTerminated())
				{
					childBehavior->DoUpdate();
				}

				const auto childResult = childBehavior->GetStatus();

				// Finish/short-circuit as soon as the policy is met
				if(childResult == BehaviorResult::Success)
				{
					++successCount;
					if(successPolicy == RequireOne)
					{
						return BehaviorResult::Success;
					}
				}

				if(childResult == BehaviorResult::Failure)
				{
					++failureCount;
					if(failurePolicy == RequireOne)
					{
						return BehaviorResult::Failure;
					}
				}
			}

			if(failurePolicy == RequireAll && failureCount == children.size())
			{
				return BehaviorResult::Failure;
			}

			if(successPolicy == RequireAll && successCount == children.size())
			{
				return BehaviorResult::Success;
			}

			return BehaviorResult::Running;
		}

		void OnTerminate() override
		{
			for(const auto childBehavior : children)
			{
				if(childBehavior->IsRunning())
				{
					childBehavior->Abort();
				}
			}
		}
	};
}
