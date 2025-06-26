#pragma once
#include <memory>

namespace gamelib
{
	class Process
	{
		friend class ProcessManager;
	public:

		enum class State : std::uint8_t
		{
			uninitialized = 0,
			removed,
			running,
			paused,
			succeeded,
			failed,
			aborted
		};

		Process() {state = State::uninitialized;}
		Process(const Process& other) = delete;
		Process(const Process&& other) = delete;
		Process& operator=(const Process& other) = delete;
		Process& operator=(const Process&& other) = delete;
		virtual ~Process() = default;

	void Succeed() { state = State::succeeded; }
	void Fail() { state = State::failed; }
	void Pause() { state = State::paused; }

	[[nodiscard]] State GetState() const { return state;  }
	[[nodiscard]] bool IsAlive() const { return state == State::running || state == State::paused; }
	[[nodiscard]] bool IsDead() const { return state == State::succeeded || state == State::failed || state == State::aborted; }
	[[nodiscard]] bool IsRemoved() const { return state == State::removed; } // Future work
	[[nodiscard]] bool IsPaused() const { return state == State::paused; } // Future work

	std::shared_ptr<Process>& AttachChild(const std::shared_ptr<Process>& newChild)
	{
		child = newChild;
		return child;
	}

	std::shared_ptr<Process> GetChild() { return child; }
	std::shared_ptr<Process> PeekChild() { return child; }

	private:
		State state;
		std::shared_ptr<Process> child;
		void SetState(const State newState) { state = newState; }
	protected:
		virtual void OnInit() { state = State::running; }
		virtual void OnUpdate(unsigned long deltaMs) = 0;
		virtual void OnSuccess() {}
		virtual void OnFail() {}
		virtual void OnAbort() {}
	};
}