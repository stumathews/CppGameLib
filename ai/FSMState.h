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

		explicit FSMState(const std::function<void()>& onEnter = nullptr, std::function<void(unsigned long deltaMs)>
		                   onUpdate = nullptr, std::function<void()> onExit = nullptr,
		                  const std::string& name = "<noname>")
		{
			if (onEnter == nullptr)
			{
				onEnterFn = []() {};
			}
			else
			{
				onEnterFn = onEnter;
			}
			
			if (onExit == nullptr)
			{
				onExit = []() {};
			}
			else
			{
				onExitFn = onExit;
			}

			if (onUpdate == nullptr)
			{
				onUpdate = [](unsigned long deltaMs) {};
			}
			else
			{
				onUpdateFn = onUpdate;
			}
		}
		void virtual OnEnter();
		void virtual OnUpdate(unsigned long deltaMs);
		void virtual OnExit();
		std::list<FSMTransition> Transitions;
		void SetOnEnter(const std::function<void()>& onEnter);
		void SetOnExit(const std::function<void()>& onExit);
		void SetOnUpdate(const std::function<void(unsigned long deltaMs)>& onUpdate);
		std::string GetName();
	private:

		std::function<void()> onEnterFn;
		std::function<void(unsigned long deltaMs)> onUpdateFn;
		std::function<void()> onExitFn;		
		std::string name;
	};
}

