#include "EngineException.h"
#include <sstream>
using namespace std;
namespace gamelib
{
	EngineException::EngineException(const int errorNumber, const std::string& message, const std::string& subsystem,
	                                 const std::string& srcFileName, const int lineNumber)
		: exception(message.c_str()), errorDescription(message), srcFileName(srcFileName),
		  subsystem(subsystem)
	{
		stringstream errorString;

		errorString << "Error: " << errorNumber << "\n"
			<< "Desc: " << errorDescription << "\n"
			<< "Src: " << srcFileName << "\n"
			<< "Line: " << lineNumber << "\n"
			<< "Subsystem: " << subsystem << "\n";
		errorText = errorString.str();
	}

	const char* EngineException::what() const
	{
		return errorText.c_str();
	}
}
