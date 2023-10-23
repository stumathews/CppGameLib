#pragma once
#include <iostream>
#include <functional>

namespace gamelib
{
	typedef std::function<void(std::string)> LoggingFunc;

	class Logger
	{			
	public:
		/// <summary>
		/// Create Logger
		/// </summary>
		static Logger* Get();
		Logger(const Logger& other) = delete;
		Logger& operator=(const Logger& other) = delete;

		/// <summary>
		/// Log message to logger
		/// </summary>
		/// <param name="message">Message to log</param>
		/// <param name="beVerbose">Log to file</param>
		/// <param name="logStdout">Log to std out</param>
		void LogThis(const std::string& message, bool beVerbose = false, bool logStdout = true) const;
	protected:
		static Logger* instance;
	private:
		LoggingFunc func;	
		void static LogToStdOut(const std::string& message);
		explicit Logger(LoggingFunc func = nullptr);
	};
}
