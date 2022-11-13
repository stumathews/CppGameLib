#pragma once
#include <memory>
#include <list>
#include <processes/Process.h>
namespace gamelib
{
	class ProcessManager
	{
	public:
		~ProcessManager() {}
		ProcessManager() {}

		unsigned int UpdateProcesses(unsigned long deltaMs);
		std::weak_ptr<Process> AttachProcess(std::shared_ptr<Process> process) { processes.push_back(process); return std::weak_ptr<Process>(process); }
		void AbortAllProcesses(bool immediate) { for (auto& process : processes) process->OnAbort(); }

		unsigned int GetProcessCount() const { return processes.size(); }
	private:

		std::list <std::shared_ptr<Process>> processes;
		void ClearAllProcesses();
	};
}