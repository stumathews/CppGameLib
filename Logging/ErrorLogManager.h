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
	ErrorLogManager(const ErrorLogManager& other) = delete;
	ErrorLogManager(const ErrorLogManager&& other) = delete;
	ErrorLogManager& operator=(const ErrorLogManager& other) = delete;
	ErrorLogManager& operator=(const ErrorLogManager&& other) = delete;
	~ErrorLogManager();
	static ErrorLogManager* GetErrorLogManager();	
	void Create(const std::string& fileName);
	void Flush();
	void Close();
	void LogException(const gamelib::EngineException& e);
	void LogMessage(const std::string& message);
	std::stringstream Buffer;
	
protected:
	ErrorLogManager() = default;
	static ErrorLogManager* instance;
	
	std::ofstream logFile;
	static std::string GetTimeString();

};

