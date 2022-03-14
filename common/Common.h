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
#include "exceptions/base_exception.h"

namespace gamelib
{
	template<typename R>
	std::shared_ptr<R> AsAsset(const std::shared_ptr<Asset> &assetInfo)
	{
		return std::static_pointer_cast<R>(assetInfo);
	}
	
	// returns true if condition succeeded, false otherwise
	inline bool IsSuccess(const bool condition, const std::string message, Logger& logger)
	{
		const auto is = condition == true;
		if(is == true)
			logger.LogThis(message);
		return is;
	}

	inline bool failed(bool condition,   Logger& logger, std::string message = "", bool ignore = false)
	{
		const auto is = condition == false;
		if(is == false)
			logger.LogThis(message);
		return ignore == true ? !ignore : is;
	}
	
	inline void LogMessage(const std::string &message, Logger& logger, const bool be_verbose = false, bool isFatal = false)
	{
		logger.LogThis(message, be_verbose);
		if(isFatal)
		{
			logger.LogThis("Fatal error encountered.", be_verbose);
			std::string key;
			std::cin >> key;
		}
	}

	// Logs message and runs action
	inline bool LogThis(const std::string &message, bool verbose, const std::function<bool()>& action,  SettingsManager& settingsAdmin, bool print_finished = true, bool run_if = true)
	{
		Logger theLogger;
		LogMessage(message, theLogger, settingsAdmin.get_bool("global","verbose"));
		bool result;
		if(run_if)
		{
			result = action();
		}
		else
		{
			result = true; // always succeed if we've been told to not run the action
		}

		
		if(print_finished)
			LogMessage("Finished.", theLogger);
		return result;
	}

	template <typename ENUM>
	auto as_integer(ENUM const value)
	{
		return static_cast<typename std::underlying_type<ENUM>::type>(value);
	}

	inline bool LogOnFailure(bool condition, std::string message, SettingsManager& settings, Logger& logger)
	{
		if(condition == false){
			
			LogMessage(message, logger, settings.get_bool("global", "verbose"));
		}
		return condition;
	}

	inline int get_random_int(const int min, const int max)
	{
		return rand() % (max - min + 1) + min;
	}
}


#endif