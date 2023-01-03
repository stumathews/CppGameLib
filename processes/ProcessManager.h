#pragma once
#include <memory>
#include <list>
#include <processes/Process.h>
namespace gamelib
{
	class ProcessManager
	{
	public:
		~ProcessManager() = default;
		ProcessManager() = default;

		unsigned int UpdateProcesses(unsigned long deltaMs);
		std::weak_ptr<Process> AttachProcess(const std::shared_ptr<Process>& process)
		{
			processes.push_back(process);
			return std::weak_ptr(process);
		}
		void AbortAllProcesses(bool immediate) const { for (auto& process : processes) process->OnAbort(); }

		[[nodiscard]] unsigned int GetProcessCount() const { return processes.size(); }
	private:

		std::list <std::shared_ptr<Process>> processes;
	};
}