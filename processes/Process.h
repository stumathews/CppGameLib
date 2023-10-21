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

		Process() {state = Uninitialized;}
		Process(const Process& other) = delete;
		Process(const Process&& other) = delete;
		Process& operator=(const Process& other) = delete;
		Process& operator=(const Process&& other) = delete;
		virtual ~Process() = default;

	void Succeed() { state = Succeeded; }
	void Fail() { state = Failed; }
	void Pause() { state = Paused; }

	[[nodiscard]] State GetState() const { return state;  }
	[[nodiscard]] bool IsAlive() const { return state == Running || state == Paused; }
	[[nodiscard]] bool IsDead() const { return state == Succeeded || state == Failed || state == Aborted; }
	[[nodiscard]] bool IsRemoved() const { return state == Removed; } // Future work
	[[nodiscard]] bool IsPaused() const { return state == Paused; } // Future work

	void AttachChild(const std::shared_ptr<Process>& newChild) { child = newChild; }
	std::shared_ptr<Process> GetChild() { return child; }
	std::shared_ptr<Process> PeekChild() { return child; }

	private:
		State state;
		std::shared_ptr<Process> child;
		void SetState(const State newState) { state = newState; }
	protected:
		virtual void OnInit() { state = Running; }
		virtual void OnUpdate(unsigned long deltaMs) = 0;
		virtual void OnSuccess() {}
		virtual void OnFail() {}
		virtual void OnAbort() {}
	};
}