#pragma once
#include <sstream>
#include "exceptions/EngineException.h"
#include <fstream>

/// <summary>
/// Logs errors
/// </summary>
class ErrorLogManager
{
public:
	static ErrorLogManager* GetErrorLogManager();	
	void Create(std::string fileName);
	void Flush();
	void Close();
	void LogException(gamelib::EngineException e);
	void LogMessage(std::string message);
	std::stringstream Buffer;
	
protected:
	ErrorLogManager();
	static ErrorLogManager* Instance;
	
	std::ofstream logFile;
	std::string GetTimeString();

};

