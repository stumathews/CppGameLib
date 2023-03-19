#pragma once
#include "Sequence.h"

namespace gamelib
{
	class Decorator : public Behavior
	{
	protected:
	    Behavior* child;

	public:
	    Decorator(Behavior* child) : child(child) {}
	};	
}
