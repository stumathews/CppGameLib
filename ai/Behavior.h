#pragma once
#include "BehaviorResult.h"

namespace gamelib
{
	/**
	 * \brief A behavior is a branch in the Behavior tree
	 */
	class Behavior
	{
	public:

		Behavior() : status(BehaviorResult::Invalid) {  }
		virtual ~Behavior() = default;

		virtual void OnInitialize(){}
		virtual BehaviorResult Update() = 0;
		virtual void OnTerminate() {}

		BehaviorResult DoUpdate()
		{
			if(status != BehaviorResult::Running)
			{
				OnInitialize();
			}

			status = Update();

			if(status != BehaviorResult::Running)
			{
				OnTerminate();
			}
			return status;
		}

		[[nodiscard]] bool IsTerminated() const { return status == BehaviorResult::Success || status == BehaviorResult::Failure; }
		[[nodiscard]] BehaviorResult GetStatus() const { return status; }
		[[nodiscard]] bool IsRunning() const { return status == BehaviorResult::Running;}
		void Abort() { OnTerminate(); status = BehaviorResult::Aborted; }
		void Reset() { status = BehaviorResult::Invalid; };

	private:
		BehaviorResult status;
	};
}
