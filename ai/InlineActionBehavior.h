#pragma once
#include <functional>

#include "Behavior.h"

namespace gamelib
{
	class InlineActionBehavior final : public Behavior
	{
	public:
		explicit InlineActionBehavior(const std::function<BehaviorResult()>& onUpdateFunc, std::string description = "")
		: Behavior(std::move(description)), onUpdateFunc(onUpdateFunc)
		{ }

		BehaviorResult Update() override
		{
			if(!onUpdateFunc) return BehaviorResult::Invalid;
			return onUpdateFunc();
		}

	private:
		const std::function<BehaviorResult()> onUpdateFunc;
	};
}
