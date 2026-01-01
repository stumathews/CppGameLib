#pragma once
#include "Status.h"
#include "Decorator.h"

namespace gamelib
{
	// A repeat decorator will repeat its child a set number of times or until failure
	class Repeat final : public Decorator
	{
	public:
		explicit Repeat(Behavior* inChild) : Decorator(inChild), limit(0), counter(0)
		{
		}

		// Sets the number of times to repeat the child behavior
	    void SetCount(const int count)
	    {
	        limit = count;
	    }

	    void OnInitialize() override
	    {
	        counter = 0;
	    }

	    Status Update(const unsigned long deltaMs) override
	    {
	        for (;;)
	        {
	            child->Tick();

	            if (child->GetStatus() == Status::Running) 
					break;

	            if (child->GetStatus() == Status::Failure) 
					return Status::Failure;

	            if (++counter == limit) 
					return Status::Success;

	            child->Reset();
	        }
	        return Status::Invalid;
	    }

	protected:
	    int limit;
	    int counter;
	};
}
