#ifndef COMMON_H
#define COMMON_H

#include <functional>
#include <memory>
#include <memory>
#include <memory>
#include <memory>
#include "asset/asset.h"
#include "file/Logger.h"
#include "Logging/ErrorLogManager.h"

namespace gamelib
{
	template<typename R>
	std::shared_ptr<R> AsAsset(const std::shared_ptr<Asset> &assetInfo)
	{
		return std::static_pointer_cast<R>(assetInfo);
	}
	
	inline bool IsSuccess(const bool condition, const std::string& message)
	{
		const auto is = condition == true;
		if (is == true)
		{		
			Logger::Get()->LogThis(message);
		}
		return is;
	}

	inline bool IsFailedOrFalse(const bool condition, const std::string& message = "", const bool ignore = false)
	{
		const auto isFalse = condition == false;
		if (isFalse == true)
		{
			Logger::Get()->LogThis(message);
			ErrorLogManager::GetErrorLogManager()->LogMessage(message);
		}
		return ignore == true ? !ignore : isFalse;
	}
	
	inline void LogMessage(const std::string &message, const bool beVerbose = false, const bool isFatal = false)
	{
		Logger::Get()->LogThis(message, beVerbose);

		if(isFatal)
		{
			Logger::Get()->LogThis("Fatal error encountered.", beVerbose);
			ErrorLogManager::GetErrorLogManager()->LogMessage(message);
		}
	}
		
	inline bool LogThis(const std::string &message, const bool _verbose, const std::function<bool()>& action,  bool print_finished = true, const bool run_if = true)
	{
		LogMessage(message, _verbose);
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

	inline bool LogOnFailure(const bool condition, const std::string& message)
	{
		if(condition == false){
			
			LogMessage(message, true);
		}
		return condition;
	}

	inline int get_random_int(const int min, const int max)
	{
		return rand() % (max - min + 1) + min;
	}
}


#endif