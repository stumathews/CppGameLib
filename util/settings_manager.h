#pragma once
#include <map>
#include <string>
#include "common/static_config.h"

namespace gamelib
{
	class setting_details
	{
	public:
		std::string name, value, type;		
		setting_details(std::string name, std::string value, std::string type);
		setting_details();
		int to_int() const;
		bool to_bool() const;
		std::string to_string() const;
		long to_long() const;
	};
	
	typedef std::map<std::string, setting_details>  settings;

	class settings_manager
	{
		std::map<std::string, settings> settings;
		const char* SETTINGS_SECTION = "settings";	
		std::string settings_file_path;
		public:
		settings_manager(){ load(); }
		bool reload();
		bool load(std::string filename_path = "settings.xml");
		bool add_setting(std::string section, std::string key, setting_details);
		setting_details get(std::string section, std::string name);
		bool get_bool(std::string section, std::string name);
		int get_int(std::string section, std::string name);
		std::string get_string(std::string section, std::string name);
		long get_long(std::string section, std::string name);
		int count() const;
	};

}
