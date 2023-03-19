#pragma once
#include "BehaviorResult.h"
#include "Composite.h"
#include "LastOrNextChildSelector.h"

namespace gamelib
{
	class FirstOrNextSelector : public LastOrNextChildSelector
	{
	protected:
		BehaviorResult Update() override
		{
			const auto prevChildBehavior = currentChildBehavior;

			LastOrNextChildSelector::OnInitialize(); // reset to select the first (hightest priority) behavior in list of child behaviors

			const auto result = LastOrNextChildSelector::Update();

			if(prevChildBehavior != children.end() && currentChildBehavior != prevChildBehavior)
			{
				(*prevChildBehavior)->Abort();
			}

			return result;
		}
	};
}
