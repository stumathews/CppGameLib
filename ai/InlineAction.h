#pragma once
#include <functional>

#include "Behavior.h"

namespace gamelib
{
	namespace ai
	{
		// This is an action that can be defined as an inline lambda call 
		class InlineAction final : public Behavior
		{
		public:
			explicit InlineAction(const std::function<Status(unsigned long deltaMs)>& onUpdateFunc, std::string description = "")
				: Behavior(std::move(description)), onUpdateFunc(onUpdateFunc)
			{
			}

			Status Update(const unsigned long deltaMs) override
			{
				if (!onUpdateFunc) return Status::Invalid;
				return onUpdateFunc(deltaMs);
			}

		private:
			std::function<Status(unsigned long deltaMs)> onUpdateFunc;
		};
	}
}
