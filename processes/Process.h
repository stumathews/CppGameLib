#pragma once
#include <memory>

namespace gamelib
{
	class Process
	{
	friend class ProcessManager;
	public:
		enum State
		{
			Uninitialized = 0,
			Removed,
			Running,
			Paused,
			Succeeded,
			Failed,
			Aborted
		};
		Process() {};
		virtual ~Process() {};

		void Succeed() { state = Succeeded; }
		inline void Fail() { state = Failed; }
		inline void Pause() { state = Paused; }
		
		State GetState() const { return state;  }
		bool IsAlive() const { return state == Running || state == Paused; }
		bool IsDead() const { return state == Succeeded || state == Failed || state == Aborted; }
		bool IsRemoved() const { return state == Removed; }
		bool IsPaused() const { return state == Paused; }

		inline void AttachChild(std::shared_ptr<Process> newChild) { child = newChild; }
		std::shared_ptr<Process> GetChild() { return child; }
		std::shared_ptr<Process> PeekChild() { return child; }

	private:
		State state;
		std::shared_ptr<Process> child;
		void SetState(State newState) { state = newState; }
	protected:
		virtual void OnInit() { state = Running; }
		virtual void OnUpdate(unsigned long delteMs) = 0;
		virtual void OnSuccess() {}
		virtual void OnFail() {}
		virtual void OnAbort() {}
	};
}