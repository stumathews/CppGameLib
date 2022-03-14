#pragma once
#include <string>

namespace gamelib
{
#ifndef THROW
#define THROW(ErrorNum, ErrorDesc, SubsystemDesc) throw EngineException(ErrorNum, ErrorDesc, SubsystemDesc,  __FILE__, __LINE__);
#endif

	class EngineException : public std::exception
	{
	public:
		EngineException(int errorNumber, std::string message, std::string subsystem, std::string srcFileName, int lineNumber);
		
		virtual const char* what() const override;
		
	private:
		int errorNumber;
		std::string errorDescription;
		std::string srcFileName;
		int lineNumber;
		std::string errorText;
		std::string subsystem;
	};
}

