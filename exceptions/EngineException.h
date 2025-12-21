#pragma once
#include <string>

namespace gamelib
{
#ifndef THROW
	/// <summary>
	/// A generic error that the Game Engine throws
	/// </summary>
	class EngineException final : public std::exception
	{
	public:
		EngineException(int errorNumber, const std::string& message, const std::string& subsystem, const std::string&
		                srcFileName, int lineNumber);

		[[nodiscard]] virtual const char* what() const noexcept override;

		~EngineException() throw() override {}
		
	private:
		std::string errorDescription;
		std::string srcFileName;
		std::string errorText;
		std::string subsystem;
	};


#define THROW(ErrorNum, ErrorDesc, SubsystemDesc) throw gamelib::EngineException(ErrorNum, ErrorDesc, SubsystemDesc,  __FILE__, __LINE__)
#endif
}

