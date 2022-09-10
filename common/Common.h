#ifndef COMMON_H
#define COMMON_H

#include <functional>
#include <memory>
#include <memory>
#include <memory>
#include <vector>
#include <memory>
#include "asset/asset.h"
#include "objects/GameObject.h"
#include "common/Logger.h"
#include "exceptions/EngineException.h"
#include "Logging/ErrorLogManager.h"

namespace gamelib
{
	template<typename R>
	std::shared_ptr<R> AsAsset(const std::shared_ptr<Asset> &assetInfo)
	{
		return std::static_pointer_cast<R>(assetInfo);
	}
	
	inline bool IsSuccess(const bool condition, const std::string message)
	{
		const auto is = condition == true;
		if (is == true)
		{		
			Logger::Get()->LogThis(message);
			ErrorLogManager::GetErrorLogManager()->LogMessage(message);
		}
		return is;
	}

	inline bool IsFailedOrFalse(bool condition, std::string message = "", bool ignore = false)
	{
		const auto isFalse = condition == false;
		if (isFalse == true)
		{
			Logger::Get()->LogThis(message);
			ErrorLogManager::GetErrorLogManager()->LogMessage(message);
		}
		return ignore == true ? !ignore : isFalse;
	}
	
	inline void LogMessage(const std::string &message, const bool be_verbose = false, bool isFatal = false)
	{
		Logger::Get()->LogThis(message, be_verbose);
		if(isFatal)
		{
			Logger::Get()->LogThis("Fatal error encountered.", be_verbose);
			ErrorLogManager::GetErrorLogManager()->LogMessage(message);
			std::string key;
			std::cin >> key;
		}
	}
		
	inline bool LogThis(const std::string &message, bool verbose, const std::function<bool()>& action,  bool print_finished = true, bool run_if = true)
	{
		LogMessage(message, SettingsManager::Get()->GetBool("global","verbose"));
		bool result;
		if(run_if)
		{
			result = action();
		}
		else
		{
			result = true; // always succeed if we've been told to not run the action
		}

		return result;
	}

	template <typename ENUM>
	auto ToInteger(ENUM const value)
	{
		return static_cast<typename std::underlying_type<ENUM>::type>(value);
	}

	inline bool LogOnFailure(bool condition, std::string message)
	{
		if(condition == false){
			
			LogMessage(message, SettingsManager::Get()->GetBool("global", "verbose"));
		}
		return condition;
	}

	inline int get_random_int(const int min, const int max)
	{
		return rand() % (max - min + 1) + min;
	}
}


#endif