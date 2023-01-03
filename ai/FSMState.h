#pragma once
#include <list>
#include <functional>
#include "FSMTransition.h"
#include <string>

namespace gamelib
{
	// ReSharper disable once CppInconsistentNaming
	class FSMState
	{
	public:
		virtual ~FSMState() = default;

		explicit FSMState(const std::function<void()>& onEnter = nullptr, std::function<void()> onUpdate = nullptr, std::function<void()> onExit = nullptr,
		                  const std::string& name = "<noname>")
		{
			if (onEnter == nullptr)
			{
				OnEnterFn = []() {};
			}
			else
			{
				OnEnterFn = onEnter;
			}
			
			if (onExit == nullptr)
			{
				onExit = []() {};
			}
			else
			{
				OnExitFn = onExit;
			}

			if (onUpdate == nullptr)
			{
				onUpdate = []() {};
			}
			else
			{
				OnUpdateFn = onUpdate;
			}
		}
		void virtual OnEnter();
		void virtual OnUpdate();
		void virtual OnExit();
		std::list<FSMTransition> Transitions;
		void SetOnEnter(const std::function<void()>& onEnter);
		void SetOnExit(const std::function<void()>& onExit);
		void SetOnUpdate(const std::function<void()>& onUpdate);
		std::string GetName();
	private:

		std::function<void()> OnEnterFn;
		std::function<void()> OnUpdateFn;
		std::function<void()> OnExitFn;		
		std::string name;
	};
}

