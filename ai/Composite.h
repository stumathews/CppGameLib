#pragma once
#include <vector>

#include "Behavior.h"

namespace gamelib
{
	// A composite behavior is a behavior that can have one or more child behaviors
	class Composite : public Behavior
	{
	public:

		// Adds a child behavior to this composite
		void AddChild(Behavior* child)
		{
			children.push_back(child);
		}

		// Removes a child behavior from this composite
		void RemoveChild(Behavior* child)
		{
			children.erase(remove(children.begin(), children.end(), child), children.end());
		}

		// Clears all child behaviors from this composite
		void ClearChildren()
		{
			children.clear();
		}

	protected:
		// A list of child behaviors
		typedef std::vector<Behavior*> Behaviors;

		// The children of this composite behavior
		Behaviors children;
	};
}
