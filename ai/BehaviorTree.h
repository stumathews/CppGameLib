#pragma once
#include "Behavior.h"

namespace gamelib
{
	class BehaviorTree
	{
	protected:
		Behavior* root;
	public:
		explicit BehaviorTree(Behavior* const root)
			: root(root)
		{
		}

		void Update(const unsigned long deltaMs = 0) const { root->DoUpdate();}
	};
}

