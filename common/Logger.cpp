#include "Logger.h"
#include "logging/ErrorLogManager.h"

using namespace std;

namespace gamelib
{
	Logger::Logger(logging_func func) : func(func) {}

	Logger* gamelib::Logger::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new Logger( [=](std::string message)
			{
				ErrorLogManager::GetErrorLogManager()->LogMessage(message);	
				LogToStdOut(message);
			});
		}
		return Instance;
	}

	Logger* gamelib::Logger::Instance = nullptr;

	void Logger::LogThis(const string& message, const bool be_verbose, const bool logToStdout) const
	{
		bool fallbackToStdOut = func == nullptr;

		// Verbose logging goes to specific function, writes to file usually
		if (be_verbose)
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
		if (logToStdout && !fallbackToStdOut)
		{
			LogToStdOut(message);
		}

		ErrorLogManager::GetErrorLogManager()->LogMessage(message);

	}

	void Logger::LogToStdOut(const string message) 	{
		cout << message << endl;
	}

}
