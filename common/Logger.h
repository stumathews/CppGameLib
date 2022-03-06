#pragma once
#include <iostream>
#include <functional>

namespace gamelib
{
	typedef std::function<void(std::string)> logging_func;
	class Logger
	{			
	public:
		/// <summary>
		/// Create Logger
		/// </summary>
		/// <param name="func"></param>
		Logger(logging_func func = nullptr);
		Logger(const Logger& other) = delete;
		Logger& operator=(const Logger& other) = delete;

		/// <summary>
		/// Log message to logger
		/// </summary>
		/// <param name="message">Message to log</param>
		/// <param name="be_verbose">Log to file</param>
		/// <param name="log_stdout">Log to std out</param>
		void LogThis(const std::string& message, const bool be_verbose = false, const bool log_stdout = true) const;
	private:
		logging_func func;	
		void LogToStdOut(const std::string message) const;
	};
}
