#pragma once
#include <string>

#include "BehaviorResult.h"

namespace gamelib
{
	/**
	 * \brief A behavior is a branch in the Behavior tree
	 */
	class Behavior  // NOLINT(cppcoreguidelines-special-member-functions)
	{
	public:
		explicit Behavior(std::string description = "")
		: Description(std::move(description)), status(BehaviorResult::Invalid)
		{
		}

		virtual ~Behavior() = default;

		// Called once immediately before the first call to the behavior's update method
		virtual void OnInitialize()
		{
		}

		// Called exactly once each time the behaviour tree is updated, until it signals it has terminated thanks to its return status
		virtual BehaviorResult Update(const unsigned long deltaMs = 0) = 0;

		// Called once immediately after the previous update signals it's no longer running.
		virtual void OnTerminate()
		{
		}

		BehaviorResult DoUpdate(const unsigned long deltaMs = 0)
		{
			// Set ourselves as running
			if (status != BehaviorResult::Running)
			{
				OnInitialize();
			}

			// Update ourselves (this is inherited by descendants)
			status = Update();

			// Check if the last update caused us to stop running, in which case call onTerminate gracefully
			if (status != BehaviorResult::Running)
			{
				OnTerminate();
			}
			return status;
		}

		[[nodiscard]] bool IsTerminated() const
		{
			return status == BehaviorResult::Success || status == BehaviorResult::Failure;
		}

		[[nodiscard]] BehaviorResult GetStatus() const { return status; }
		[[nodiscard]] bool IsRunning() const { return status == BehaviorResult::Running; }

		void Abort()
		{
			OnTerminate();
			status = BehaviorResult::Aborted;
		}

		void Reset() { status = BehaviorResult::Invalid; }

		std::string Description;

	private:
		BehaviorResult status;
	};
}
