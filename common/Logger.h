#pragma once
#include <iostream>
#include <functional>

namespace gamelib
{
	typedef std::function<void(std::string)> logging_func;
	class Logger
	{
		logging_func func;		
	public:
		Logger(logging_func func = nullptr) : func(func) {}
		Logger(const Logger& other) = delete;
		Logger& operator=(const Logger& other) = delete;

		void log_message(const std::string &message, const bool be_verbose = false) const
		{
			if (be_verbose) {
				if(func != nullptr)
					func(message);
				else
					std::cout << message << std::endl;
			}
		}
	};
}
