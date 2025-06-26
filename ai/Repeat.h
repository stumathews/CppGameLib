#pragma once
#include "BehaviorResult.h"
#include "Decorator.h"

namespace gamelib
{
	class Repeat final : public Decorator
	{
	public:
		explicit Repeat(Behavior* inChild) : Decorator(inChild), limit(0), counter(0)
		{
		}

	    void SetCount(const int count)
	    {
	        limit = count;
	    }

	    void OnInitialize() override
	    {
	        counter = 0;
	    }

	    BehaviorResult Update() override 
	    {
	        for (;;)
	        {
	            child->DoUpdate();
	            if (child->GetStatus() == BehaviorResult::Running) 
					break;
	            if (child->GetStatus() == BehaviorResult::Failure) 
					return BehaviorResult::Failure;
	            if (++counter == limit) 
					return BehaviorResult::Success;
	            child->Reset();
	        }
	        return BehaviorResult::Invalid;
	    }

	protected:
	    int limit;
	    int counter;
	};
}
