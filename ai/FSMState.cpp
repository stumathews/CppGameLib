#include "FSMState.h"

using namespace std;

namespace gamelib
{
	void FSMState::OnEnter() 
	{
		// Run passed in lambda if its provided
		if (onEnterFn != nullptr)
		{
			onEnterFn();
		}
	}

	void FSMState::OnUpdate() 
	{
		// Run passed in lambda if its provided
		if (onUpdateFn != nullptr)
		{
			onUpdateFn();
		}
	}

	void FSMState::OnExit() 
	{
		// Run passed in lambda if its provided
		if (onExitFn != nullptr)
		{
			onExitFn();
		}
	}

	void FSMState::SetOnEnter(const std::function<void()>& onEnter)
	{
		onEnterFn = onEnter;
	}

	void FSMState::SetOnExit(const std::function<void()>& onExit)
	{
		onExitFn = onExit;
	}
	void FSMState::SetOnUpdate(const std::function<void()>& onUpdate)
	{
		onUpdateFn = onUpdate;
	}

	string FSMState::GetName()
	{
		return name;
	}
}