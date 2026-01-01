#pragma once
#include "Status.h"
#include "Composite.h"

namespace gamelib
{
	// A parallel composite runs all child behaviors simultaneously
	class Parallel : public Composite
	{
	public:

		// The policy for determining success or failure
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

		virtual Status Update(const unsigned long deltaMs) override
		{
			size_t successCount = 0, failureCount = 0;

			// Run each child behavior in the same frame
			for(const auto childBehavior : children)
			{
				if(!childBehavior->IsTerminated())
				{
					childBehavior->Tick();
				}

				const auto childResult = childBehavior->GetStatus();

				// Finish/short-circuit as soon as the policy is met
				if(childResult == Status::Success)
				{
					++successCount;

					if(successPolicy == Policy::require_one)
					{
						return Status::Success;
					}
				}

				if(childResult == Status::Failure)
				{
					++failureCount;
					if(failurePolicy == Policy::require_one)
					{
						return Status::Failure;
					}
				}
			}

			if(failurePolicy == Policy::require_all && failureCount == children.size())
			{
				return Status::Failure;
			}

			if(successPolicy == Policy::require_all && successCount == children.size())
			{
				return Status::Success;
			}

			return Status::Running;
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
