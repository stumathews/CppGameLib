#include "Logger.h"
#include "logging/ErrorLogManager.h"

using namespace std;

namespace gamelib
{
	Logger::Logger(LoggingFunc func) : func(std::move(func)) {}

	Logger* Logger::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new Logger( [=](const std::string& message)
			{
				ErrorLogManager::GetErrorLogManager()->LogMessage(message);	
				LogToStdOut(message);
			});
		}
		return Instance;
	}

	Logger* Logger::Instance = nullptr;

	void Logger::LogThis(const string& message, const bool beVerbose, const bool logStdout) const
	{
		bool fallbackToStdOut = func == nullptr;

		// Verbose logging goes to specific function, writes to file usually
		if (beVerbose)
		{
			if (!fallbackToStdOut)
			{
				fallbackToStdOut = true;

				// use logging function
				func(message);
			}
			else
			{
				// Fallback to std output
				LogToStdOut(message);
			}
		}

		// Explicitly asked to write to std out
		if (logStdout && !fallbackToStdOut)
		{
			LogToStdOut(message);
		}

		ErrorLogManager::GetErrorLogManager()->LogMessage(message);

	}

	void Logger::LogToStdOut(const string& message) 	{
		cout << message << endl;
	}

}
