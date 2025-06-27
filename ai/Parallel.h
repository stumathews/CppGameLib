#pragma once
#include "BehaviorResult.h"
#include "CompositeBehavior.h"

namespace gamelib
{
	class Parallel : public CompositeBehavior
	{
	public:
		enum class Policy : uint8_t
		{
			require_one,
			require_all
		};
		Parallel(const Policy success, const Policy failure): successPolicy(success), failurePolicy(failure){}
		~Parallel() override = default;

	protected:
		Policy successPolicy;
		Policy failurePolicy;
		virtual BehaviorResult Update(const unsigned long deltaMs) override
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
					if(successPolicy == Policy::require_one)
					{
						return BehaviorResult::Success;
					}
				}

				if(childResult == BehaviorResult::Failure)
				{
					++failureCount;
					if(failurePolicy == Policy::require_one)
					{
						return BehaviorResult::Failure;
					}
				}
			}

			if(failurePolicy == Policy::require_all && failureCount == children.size())
			{
				return BehaviorResult::Failure;
			}

			if(successPolicy == Policy::require_all && successCount == children.size())
			{
				return BehaviorResult::Success;
			}

			return BehaviorResult::Running;
		}

		void OnTerminate() override
		{
			// If we're terminating, we should terminate any running child behaviors 
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
