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

		// Calculate the Utility of this branch
		float CalculateUtility() override
		{
			// We'll use the child with the largest Utility to indicate the utility of this branch
			auto largestChildUtility = 0.0f;

			for (auto child = children.begin(); child != children.end(); ++child)
			{
				// Keep track of the largest utility for far
				auto childUtility = (*child)->CalculateUtility();
				if ( childUtility > largestChildUtility )
				{
					largestChildUtility = childUtility;
				}
			}

			// Return our utility value as the largest utility value found in the children
			return largestChildUtility;
		}

	protected:
		// A list of child behaviors
		typedef std::vector<Behavior*> Behaviors;

		// The children of this composite behavior
		Behaviors children;
	};
}
