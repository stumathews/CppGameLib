#pragma once
#include <vector>

#include "Behavior.h"

namespace gamelib
{
	class CompositeBehavior : public Behavior
	{
	public:

		/**
		 * \brief list of behaviors, higher priority behaviors occur before lower ones
		 * \param child next priority behavior
		 */
		void AddChild(Behavior* child)
		{
			children.push_back(child);
		}

		void RemoveChild(Behavior* child)
		{
			children.erase(remove(children.begin(), children.end(), child));
		}

		void ClearChildren()
		{
			children.clear();
		}

	protected:
		typedef std::vector<Behavior*> Behaviors;
		Behaviors children;
	};
}
