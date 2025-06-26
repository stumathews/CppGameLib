#include <processes/ProcessManager.h>

using namespace std;

namespace gamelib
{
	unsigned int ProcessManager::UpdateProcesses(const unsigned long deltaMs)
	{
		unsigned short int successCount = 0;
		unsigned short int failCount = 0;

		auto it = processes.begin();
		while (it != processes.end())
		{
			const shared_ptr<Process> currentProcess = (*it);
			const auto thisIt = it;
			++it;
			if (currentProcess->GetState() == Process::State::uninitialized) { currentProcess->OnInit(); }
			if (currentProcess->GetState() == Process::State::running) { currentProcess->OnUpdate(deltaMs); }

			if (currentProcess->IsDead())
			{
				switch (currentProcess->GetState())
				{
					case Process::State::succeeded:
					{
						currentProcess->OnSuccess();
						shared_ptr<Process> child = currentProcess->GetChild();
						if (child)
						{
							AttachProcess(child);
						}
						else
						{
							++successCount;
						}
						break;
					}
					case Process::State::failed:
					{
						currentProcess->OnFail();
						++failCount;
						break;
					}
				}
				processes.erase(thisIt);
			}
		}
		return ((successCount << 16 | failCount));
	}

	std::weak_ptr<Process> ProcessManager::AttachProcess(const std::shared_ptr<Process>& process)
	{
		processes.push_back(process);
		return std::weak_ptr(process);
	}
}
