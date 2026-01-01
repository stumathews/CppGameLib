#pragma once

namespace gamelib
{
	// A decorator is an augmented behavior that has a single unaugmented child behavior
	class Decorator : public Behavior
	{
	protected:
	    Behavior* child;

	public:
	    Decorator(Behavior* child) : child(child) {}
	    Status Update(unsigned long deltaMs) override = 0;
	};
}
