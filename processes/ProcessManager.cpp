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
			if (currentProcess->GetState() == Process::Uninitialized) { currentProcess->OnInit(); }
			if (currentProcess->GetState() == Process::Running) { currentProcess->OnUpdate(deltaMs); }

			if (currentProcess->IsDead())
			{
				switch (currentProcess->GetState())
				{
					case Process::Succeeded:
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
					case Process::Failed:
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
