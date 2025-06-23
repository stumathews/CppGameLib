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

		void Tick() const { root->DoUpdate();}
	};
}

