#include "EngineException.h"
#include <sstream>
using namespace std;
namespace gamelib
{
	EngineException::EngineException(int errorNumber, std::string message, std::string subsystem, std::string srcFileName, int lineNumber)
		: errorNumber(errorNumber), errorDescription(message), srcFileName(srcFileName), lineNumber(lineNumber), subsystem(subsystem), exception(message.c_str())
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
