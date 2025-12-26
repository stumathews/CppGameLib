#pragma once
#include <functional>

#include "Behavior.h"

namespace gamelib
{
	// This is an action that can be defined as an inline lambda call 
	class InlineBehavioralAction final : public Behavior
	{
	public:
		explicit InlineBehavioralAction(const std::function<BehaviorResult(unsigned long deltaMs)>& onUpdateFunc, std::string description = "")
		: Behavior(std::move(description)), onUpdateFunc(onUpdateFunc)
		{ }

		BehaviorResult Update(const unsigned long deltaMs) override
		{
			if(!onUpdateFunc) return BehaviorResult::Invalid;
			return onUpdateFunc(deltaMs);
		}

	private:
		std::function<BehaviorResult(unsigned long deltaMs)> onUpdateFunc;
	};
}
