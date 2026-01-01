#pragma once
#include "ai/BehaviorTree.h"
#include "ai/ActiveSelector.h"
#include "ai/Sequence.h"
#include <stack>

#include "Filter.h"
#include "Parallel.h"

class BehaviorTreeBuilder
{
public:
	BehaviorTreeBuilder() = default;

	// Adds an ActiveSelector composite to the behavior tree
	BehaviorTreeBuilder& ActiveSelector()
	{
		AddChildToCurrentNode(new gamelib::ActiveSelector());
		return *this;
	}

	// Adds a Sequence composite to the behavior tree
	BehaviorTreeBuilder& Sequence(const char* description = "")
	{
		AddChildToCurrentNode(new gamelib::ai::Sequence());
		return *this;
	}

	// Adds a Condition behavior to the behavior tree
	BehaviorTreeBuilder& Condition(gamelib::Behavior* condition)
	{
		CurrentNode()->AddChild(condition);
		return *this;
	}

	// Adds an Action behavior to the behavior tree
	BehaviorTreeBuilder& Action(gamelib::Behavior* action)
	{
		CurrentNode()->AddChild(action);
		return *this;
	}

	// Adds a Filter composite to the behavior tree
	BehaviorTreeBuilder& Filter(gamelib::Filter* filter)
	{
		AddChildToCurrentNode(filter);
		return *this;
	}

	// Adds a Parallel composite to the behavior tree
	BehaviorTreeBuilder& Parallel(const gamelib::Parallel::Policy successPolicy, const gamelib::Parallel::Policy failurePolicy)
	{
		// Not Tested.
		AddChildToCurrentNode(new gamelib::Parallel(successPolicy, failurePolicy));
		return *this;
	}

	// Finalizes the behavior tree and returns the constructed BehaviorTree object
	[[nodiscard]] gamelib::BehaviorTree* End() const
	{
		return new gamelib::BehaviorTree(root);
	}

	// Finishes the current composite and moves back up to the parent node
	BehaviorTreeBuilder& Finish()
	{
		if(!tree.empty())
			tree.pop();

		return *this;
	}

private:

	// The root of the behavior tree
	gamelib::Composite* root = nullptr;

	// The stack used to keep track of the current node in the tree
	std::stack<gamelib::Composite*> tree;

	// Adds a child behavior to the current node in the tree
	void AddChildToCurrentNode(gamelib::Composite* behavior)
	{
		// If the tree is empty, set the root to the new behavior
		if(root == nullptr)
		{
			root = behavior;
			tree.push(root);
			return;
		}

		// Otherwise, add the new behavior as a child of the current node
		tree.top()->AddChild(behavior);
		tree.push(behavior);
	}

	// Returns the current node in the tree
	gamelib::Composite* CurrentNode()
	{
		return tree.top();
	}
	
};

