#include "ErrorLogManager.h"
#include <iomanip>
#include <time/time.h>

ErrorLogManager::~ErrorLogManager()
{
	Flush();
	Close();
}

ErrorLogManager* ErrorLogManager::GetErrorLogManager()
{
	if (instance == nullptr)
	{
		instance = new ErrorLogManager();
	}
	return instance;
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


std::string ErrorLogManager::GetTimeString()
{
	std::stringstream timeString;
	const time_t now = time(nullptr);
	const std::tm newTime  = gamelib::localtime_safe(now);

	timeString << std::setw(2) << std::setfill('0') << newTime.tm_mday << "-";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_mon << "-";
	timeString << std::setw(2) << std::setfill('0') << 1900 + newTime.tm_year << " ";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_hour << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_min << ":";
	timeString << std::setw(2) << std::setfill('0') << newTime.tm_sec;

	return timeString.str();
}

ErrorLogManager* ErrorLogManager::instance = nullptr;
