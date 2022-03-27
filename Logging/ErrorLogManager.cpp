#include "ErrorLogManager.h"
#include <iomanip>

ErrorLogManager* ErrorLogManager::GetErrorLogManager()
{
	if (Instance == nullptr)
	{
		Instance = new ErrorLogManager();
	}
	return Instance;
}

void ErrorLogManager::Create(std::string fileName)
{
	logFile.open(fileName.c_str());
}

void ErrorLogManager::Flush()
{
	logFile << Buffer.str();
	logFile.flush();
	Buffer.str("");
}

void ErrorLogManager::Close()
{
	logFile.close();
}

void ErrorLogManager::LogException(gamelib::EngineException e)
{
	Buffer << GetTimeString() << "\n" << e.what();
	Flush();
}

ErrorLogManager::ErrorLogManager()
{
}

std::string ErrorLogManager::GetTimeString()
{
	std::stringstream timeString;
	struct tm newTime;
	time_t now = time(0);

	localtime_s(&newTime, &now);

	timeString << std::setw(2) << std::setfill('0') << newTime.tm_hour << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_min << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_sec;

	return timeString.str();
}

ErrorLogManager* ErrorLogManager::Instance = nullptr;
