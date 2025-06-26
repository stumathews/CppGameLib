#pragma once
#include "ai/BehaviorTree.h"
#include "ai/ActiveSelector.h"
#include "ai/BehavioralSequence.h"
#include <stack>

class BehaviorTreeBuilder
{
public:
	BehaviorTreeBuilder() = default;

	BehaviorTreeBuilder& ActiveNodeSelector()
	{
		AddChildToCurrentNode(new gamelib::ActiveSelector());
		return *this;
	}

	BehaviorTreeBuilder& Sequence(const char* description = "")
	{
		AddChildToCurrentNode(new gamelib::BehavioralSequence());
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
		AddChildToCurrentNode(new gamelib::BehavioralSequence());
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
	
	enum class BehaviorType : int8_t
	{
		none,
		seq,
		as,
		cond,
		filter,
		act

	};

	void AddChildToCurrentNode(gamelib::CompositeBehavior* behavior)
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

	gamelib::CompositeBehavior* CurrentNode()
	{
		return tree.top();
	}
	
	gamelib::CompositeBehavior* root = nullptr;
	std::stack<gamelib::CompositeBehavior*> tree;
	gamelib::Behavior* currentNodePtr = nullptr;
	gamelib::Behavior* prevNodePtr = nullptr;
		

};

