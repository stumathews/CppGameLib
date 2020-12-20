#ifndef COMMON_H
#define COMMON_H

#include <functional>
#include <SDL.h>
#include <vector>
#include <memory>
#include "asset/asset.h"
#include "objects/game_object.h"
#include "common/Logger.h"

namespace gamelib
{
	template<typename R>
	std::shared_ptr<R> as_resource(const std::shared_ptr<asset> &the_asset)
	{
		return std::static_pointer_cast<R>(the_asset);
	}

	inline bool succeeded(const bool condition, const std::string message)
	{
		const auto is = condition == true;
		if(is == true)
			logger::log_message(message);
		return is;
	}

	inline bool failed(bool condition,  std::string message = "", bool ignore = false)
	{
		const auto is = condition == false;
		if(is == false)
			logger::log_message(message);
		return ignore == true ? !ignore : is;
	}

	struct game_world_data 
	{
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		
		bool is_game_done = false;
		bool is_network_game = false;
		bool can_render = true;
			
		// List of game objects
		std::vector<std::shared_ptr<game_object>> game_objects;
	};

	inline void log_message(const std::string &message, const bool be_verbose = global_config::verbose, bool is_fatal = false)
	{
		logger::log_message(message, be_verbose);
		if(is_fatal)
		{
			logger::log_message("Fatal error encountered.", be_verbose);
			std::string key;
			std::cin >> key;
		}
	}

	inline bool run_and_log(const std::string &message, bool verbose, const std::function<bool()>& action, bool print_finished = true, bool run_if = true)
	{
		log_message(message);
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

	inline bool log_if_false(bool condition, std::string message)
	{
		if(condition == false)
			log_message(message);
		return condition;
	}
}


#endif