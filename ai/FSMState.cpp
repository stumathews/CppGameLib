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

	void FSMState::OnUpdate(unsigned long deltaMs) 
	{
		// Run passed in lambda if its provided
		if (onUpdateFn != nullptr)
		{
			onUpdateFn(deltaMs);
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
	void FSMState::SetOnUpdate(const std::function<void(unsigned long deltaMs)>& onUpdate)
	{
		onUpdateFn = onUpdate;
	}

	string FSMState::GetName()
	{
		return name;
	}
}