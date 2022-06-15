#include "FSMState.h"

using namespace std;

namespace gamelib
{
	void FSMState::OnEnter() 
	{
		// Run passed in lambda if its provided
		if (OnEnterFn != nullptr)
		{
			OnEnterFn();
		}
	}
	void FSMState::OnUpdate() 
	{
		// Run passed in lambda if its provided
		if (OnUpdateFn != nullptr)
		{
			OnUpdateFn();
		}
	}
	void FSMState::OnExit() 
	{
		// Run passed in lambda if its provided
		if (OnExitFn != nullptr)
		{
			OnExitFn();
		}
	};

	void FSMState::SetOnEnter(std::function<void()> onEnter)
	{
		OnEnterFn = onEnter;
	}

	void FSMState::SetOnExit(std::function<void()> onExit)
	{
		OnExitFn = onExit;
	}
	void FSMState::SetOnUpdate(std::function<void()> onUpdate)
	{
		OnUpdateFn = onUpdate;
	}

	string FSMState::GetName()
	{
		return name;
	}
}