#pragma once
#include <sstream>
#include "exceptions/base_exception.h"
#include <fstream>

class ErrorLogManager
{
public:
	static ErrorLogManager* GetErrorLogManager();	
	void Create(std::string fileName);
	void Flush();
	void Close();
	void LogException(gamelib::EngineException e);
	std::stringstream Buffer;
	
protected:
	ErrorLogManager();
	static ErrorLogManager* Instance;
	
	std::ofstream logFile;
	std::string GetTimeString();

};

