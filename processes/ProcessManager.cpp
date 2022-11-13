#include <processes/ProcessManager.h>
using namespace std;
namespace gamelib
{
	unsigned int ProcessManager::UpdateProcesses(unsigned long deltaMs)
	{
		unsigned short int successCount = 0;
		unsigned short int failCount = 0;

		list<shared_ptr<Process>>::iterator it = processes.begin();
		while (it != processes.end())
		{
			shared_ptr<Process> currProcess = (*it);
			list<shared_ptr<Process>>::iterator thisIt = it;
			++it;
			if (currProcess->GetState() == Process::Uninitialized)
				currProcess->OnInit();
			if (currProcess->GetState() == Process::Running)
				currProcess->OnUpdate(deltaMs);

			if (currProcess->IsDead())
			{
				switch (currProcess->GetState())
				{
					case Process::Succeeded:
					{
						currProcess->OnSuccess();
						shared_ptr<Process> child = currProcess->GetChild();
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
						currProcess->OnFail();
						++failCount;
						break;
					}
				}
				processes.erase(thisIt);
			}
		}
		return ((successCount << 16 | failCount));
	}
}
