#include "EngineException.h"
#include <sstream>

using namespace std;

namespace gamelib
{
	EngineException::EngineException(const int errorNumber, const std::string& message, const std::string& subsystem,
	                                 const std::string& srcFileName, const int lineNumber)
		: exception(), errorDescription(message), srcFileName(srcFileName),
		  subsystem(subsystem)
	{
		stringstream errorString;

		errorString << "Error: " << errorNumber << "\n"
			<< "Desc: " << errorDescription << "\n"
			<< "Src: " << srcFileName << "\n"
			<< "Line: " << lineNumber << "\n"
			<< "Subsystem: " << subsystem;
		
		errorText = errorString.str();
	}

	const char* EngineException::what() const throw()
	{
		return errorText.c_str();
	}
}
