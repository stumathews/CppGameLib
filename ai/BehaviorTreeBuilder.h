#pragma once
#include "ai/BehaviorTree.h"
#include "ai/ContinueLastSuccessfulSequenceSelector.h"
#include "ai/ShortCircuitSequence.h"
#include <stack>

class BehaviorTreeBuilder
{
public:
	BehaviorTreeBuilder() = default;

	BehaviorTreeBuilder& ActiveNodeSelector()
	{
		AddChildToCurrentNode(new gamelib::ContinueLastSuccessfulSequenceSelector());
		return *this;
	}

	BehaviorTreeBuilder& Sequence(const char* description = "")
	{
		AddChildToCurrentNode(new gamelib::ShortCircuitSequence());
		return *this;
	}

	[[nodiscard]] gamelib::BehaviorTree* End() const
	{
		return new gamelib::BehaviorTree(root);
	}
	BehaviorTreeBuilder& Condition(gamelib::Behavior* condition)
	{
		CurrentNode()->AddChild(condition);
		return *this;
	}
	BehaviorTreeBuilder& Filter()
	{
		AddChildToCurrentNode(new gamelib::ShortCircuitSequence());
		return *this;
	}
	BehaviorTreeBuilder& Action(gamelib::Behavior* action)
	{
		CurrentNode()->AddChild(action);

		return *this;
	}

	BehaviorTreeBuilder& Finish()
	{
		if(!tree.empty())
			tree.pop();
		return *this;
	}

private:
	
	enum BehaviorType
	{
		NONE,
		SEQ,
		AS,
		COND,
		FILTER,
		ACT

	};

	void AddChildToCurrentNode(gamelib::Composite* behavior)
	{
		if(root == nullptr)
		{
			root = behavior;
			tree.push(root);
			return;
		}

		tree.top()->AddChild(behavior);
		tree.push(behavior);
	}

	gamelib::Composite* CurrentNode()
	{
		return tree.top();
	}
	
	gamelib::Composite* root = nullptr;
	std::stack<gamelib::Composite*> tree;

};

