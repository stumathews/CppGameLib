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

void ErrorLogManager::Create(const std::string& fileName)
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

void ErrorLogManager::LogException(const gamelib::EngineException& e)
{
	Buffer << GetTimeString() << "\n" << e.what() << std::endl;
	Flush();
}

void ErrorLogManager::LogMessage(const std::string& message)
{
	Buffer << GetTimeString() << " " << message << std::endl;
	Flush();
}

ErrorLogManager::ErrorLogManager() = default;

std::string ErrorLogManager::GetTimeString()
{
	std::stringstream timeString;
	tm newTime{};
	const time_t now = time(nullptr);

	localtime_s(&newTime, &now);  // NOLINT(cert-err33-c)

	timeString << std::setw(2) << std::setfill('0') << newTime.tm_mday << "-";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_mon << "-";
	timeString << std::setw(2) << std::setfill('0') << 1900 + newTime.tm_year << " ";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_hour << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_min << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_sec;

	return timeString.str();
}

ErrorLogManager* ErrorLogManager::Instance = nullptr;
