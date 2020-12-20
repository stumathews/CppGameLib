#pragma once
#include <string>

namespace gamelib
{
	class base_exception : public std::exception
	{
	public:
		base_exception(std::string message, std::string subsystem);
		std::string subsystem;
		virtual std::string get_source_subsystem() const {return subsystem;}
	};
}

