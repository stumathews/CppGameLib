#pragma once

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
