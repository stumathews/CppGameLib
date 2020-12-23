#pragma once
#include <iostream>
#include <functional>

namespace gamelib
{
	typedef std::function<void(std::string)> logging_func;
	class logger
	{
		logging_func func;		
	public:
		logger(logging_func func = nullptr) : func(func) {}
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
