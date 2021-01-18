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

namespace gamelib
{
	template<typename R>
	std::shared_ptr<R> as_resource(const std::shared_ptr<asset> &the_asset)
	{
		return std::static_pointer_cast<R>(the_asset);
	}

	inline bool succeeded(const bool condition, const std::string message, std::shared_ptr<logger> the_logger = std::make_shared<logger>())
	{
		const auto is = condition == true;
		if(is == true)
			the_logger->log_message(message);
		return is;
	}

	inline bool failed(bool condition,  std::string message = "", bool ignore = false, std::shared_ptr<logger> the_logger = std::make_shared<logger>())
	{
		const auto is = condition == false;
		if(is == false)
			the_logger->log_message(message);
		return ignore == true ? !ignore : is;
	}
	
	inline void log_message(const std::string &message, const bool be_verbose = false, bool is_fatal = false, const std::shared_ptr<logger> the_logger = std::make_shared<logger>())
	{
		the_logger->log_message(message, be_verbose);
		if(is_fatal)
		{
			the_logger->log_message("Fatal error encountered.", be_verbose);
			std::string key;
			std::cin >> key;
		}
	}

	inline bool run_and_log(const std::string &message, bool verbose, const std::function<bool()>& action, bool print_finished = true, bool run_if = true, std::shared_ptr<settings_manager> settings_admin = std::make_shared<settings_manager>())
	{
		log_message(message, settings_admin->get_bool("global","verbose"));
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
			log_message("Finished.");
		return result;
	}

	template <typename ENUM>
	auto as_integer(ENUM const value)
	{
		return static_cast<typename std::underlying_type<ENUM>::type>(value);
	}

	inline bool log_if_false(bool condition, std::string message, std::shared_ptr<settings_manager> settings = std::make_shared<settings_manager>(), std::shared_ptr<logger> the_logger = std::make_shared<logger>())
	{
		if(condition == false){
			
			log_message(message, settings->get_bool("global","verbose"), false, the_logger);
		}
		return condition;
	}

	inline int get_random_int(const int min, const int max)
	{
		return rand() % (max - min + 1) + min;
	}
}


#endif