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
			explicit InlineAction(const std::function<Status(unsigned long deltaMs)>& onUpdateFunc, std::string description = "", std::function<float(void)> calculateUtilityFunc = nullptr)
				: Behavior(std::move(description)), onUpdateFunc(onUpdateFunc),
				  calculate_utility_func(std::move(calculateUtilityFunc))
			{
			}

			Status Update(const unsigned long deltaMs) override
			{
				if (!onUpdateFunc) return Status::Invalid;
				return onUpdateFunc(deltaMs);
			}

			float CalculateUtility() override
			{
				if (calculate_utility_func == nullptr) return 0.0f;

				return calculate_utility_func();
			}

		private:
			std::function<Status(unsigned long deltaMs)> onUpdateFunc;
			std::function<float()> calculate_utility_func;
		};
	}
}
