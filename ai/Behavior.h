#pragma once
#include <string>

#include "Status.h"

namespace gamelib
{
	/**
	 * \brief A behavior is a branch in the Behavior tree
	 */
	class Behavior
	{
	public:
		virtual ~Behavior() = default;

		explicit Behavior(std::string description = "")
		: Description(std::move(description)), status(Status::Invalid)
		{
		}

		// Called once immediately before the first call to the behavior's update method
		virtual void OnInitialize() 
		{
		}

		// Called exactly once each time the behaviour tree is updated, until it signals it has terminated thanks to its return status
		virtual Status Update(unsigned long deltaMs) = 0;

		// Called once immediately after the previous update signals it's no longer running.
		virtual void OnTerminate()
		{
		}

		// Advances the behavior by one tick
		Status Tick(const unsigned long deltaMs = 0)
		{
			// Set ourselves as running
			if (status != Status::Running)
			{
				OnInitialize();
			}

			// Update ourselves (this is inherited by descendants)
			status = Update(deltaMs);

			// Check if the update caused us to stop running, in which case call onTerminate gracefully
			if (status != Status::Running)
			{
				OnTerminate();
			}
			return status;
		}

		// Returns true if the behavior has terminated (success or failure)
		[[nodiscard]] bool IsTerminated() const
		{
			return status == Status::Success || status == Status::Failure;
		}

		// Returns the current status of the behavior
		[[nodiscard]] Status GetStatus() const { return status; }

		// Returns true if the behavior is currently running
		[[nodiscard]] bool IsRunning() const { return status == Status::Running; }

		// Aborts the behavior immediately
		void Abort()
		{
			OnTerminate();
			status = Status::Aborted;
		}

		// Resets the behavior to its initial state
		void Reset() { status = Status::Invalid; }

		// A description of the behavior
		std::string Description;

	private:
		Status status;
	};
}
