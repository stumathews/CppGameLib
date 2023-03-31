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

		explicit FSMState(std::string name = "<NoStateName>", 						  
		                  std::function<void(unsigned long deltaMs)> onUpdate = NoUpdate,
		                  std::function<void()> onEnter = DoNothing,
		                  std::function<void()> onExit = DoNothing
		) : onEnterFn(std::move(onEnter)), onUpdateFn(std::move(onUpdate)), onExitFn(std::move(onExit)), name(std::move(name)){}
		
		void virtual OnEnter();
		void virtual OnUpdate(unsigned long deltaMs);
		void virtual OnExit();
		std::list<FSMTransition> Transitions;
		void SetOnEnter(const std::function<void()>& onEnter);
		void SetOnExit(const std::function<void()>& onExit);
		void SetOnUpdate(const std::function<void(unsigned long deltaMs)>& onUpdate);
		std::string GetName();

		static void DoNothing(){}
		static void NoUpdate(unsigned long) {}

	private:
		
		std::function<void()> onEnterFn;
		std::function<void(unsigned long deltaMs)> onUpdateFn;
		std::function<void()> onExitFn;
		std::string name;
	};
}
