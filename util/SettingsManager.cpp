#include "SettingsManager.h"
#include "tinyxml2.h"
using namespace std;

namespace gamelib
{
	setting_details::setting_details(std::string name, std::string value, std::string type) : name(name), value(value), type(type)
	{
	}

	setting_details::setting_details(): name("empty"), value("empty"), type("empty")
	{
	}

	int setting_details::to_int() const
	{
		if(type == "int")
			return std::atoi(value.c_str());
		throw exception("Setting is not an integer");
	}

	bool setting_details::to_bool() const
	{
		if(type == "bool")
			return value == "true" ? true : false;
		throw exception("Setting is not a bool");
	}

	std::string setting_details::to_string() const
	{
		if(type == "string")
			return value;
		throw exception("Setting is not a string");
	}

	long setting_details::to_long() const
	{
		if(type == "long")
			return std::strtol(value.c_str(), nullptr,10);
		throw exception("Setting is not a long");
	}

	bool SettingsManager::reload()
	{
		return load(settings_file_path);
	}

	bool SettingsManager::load(string filename_path)
	{
		this->settings_file_path = filename_path;
		settings.clear(); // effectively this is reloading the settings from scratch each time
		
		tinyxml2::XMLDocument doc;
		
		/*
			<settings>
				<global>
					<!-- 20 times a second = 50 milliseconds, 1 second is 20*50 = 1000 milliseconds -->
					<setting name="tick_time_ms" type="int">50</setting>
					<setting name="square_width" type="int">30</setting>
					<setting name="sprite_width" type="int">100</setting>
					<setting name="max_loops" type="int">4</setting>
					<setting name="screen_width" type="int">800</setting>
					<setting name="screen_height" type="int">600</setting>
					<setting name="frames_per_row" type="int">3</setting>
					<setting name="frames_per_column" type="int">3</setting>
					<setting name="verbose" type="bool">true</setting>
					<setting name="use_3d_render_manager" type="bool">true</setting>
				</global>
				<player>
					<setting name="player_init_pos_x" type="int">100</setting>
					<setting name="player_init_pos_y" type="int">100</setting>
					<setting name="move_interval" type="int">2</setting>
				</player>
			</settings>
		 */

		
		if(doc.LoadFile(filename_path.c_str()) == tinyxml2::XML_SUCCESS)
		{
			// Loopt through settings section
			for(auto section = doc.FirstChildElement(SETTINGS_SECTION)->FirstChild(); section; section = section->NextSibling())
			{	
				if(section->ToComment())
					continue;

				// Loopt through each setting section eg. 'global' or 'player' etc.
				const auto* section_name = section->Value();
				for(auto* setting = section->FirstChild(); setting; setting = setting->NextSibling())
				{
					if(setting->ToComment())
					continue;

					// Loop through the settings attributes i.e name, type etc.
					auto setting_element = setting->ToElement();
					string setting_name;
					string setting_value;
					string setting_type;
					for(const auto* attributes = setting_element->FirstAttribute(); attributes; attributes = attributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
					{
						const string key(attributes->Name());
						const string value(attributes->Value());						

						if(key == "name")
							setting_name = value;
						if(key == "type")
							setting_type = value;
					}

					// Persist the setting into the SettingsManager
					add_setting(section_name, setting_name,  setting_details(setting_name, setting_element->GetText(), setting_type));
				}
			}
		}
		else
		{
			return false;
		}
		return true;
	}

	bool SettingsManager::add_setting(std::string section, std::string key, setting_details details)
	{
		settings[section][key] = details;
		return true;
	}

	setting_details SettingsManager::get(std::string section, std::string name)
	{
		return settings[section][name];
	}
	
	bool SettingsManager::get_bool(std::string section, std::string name)
	{
		return settings[section][name].to_bool();
	}
	
	int SettingsManager::get_int(std::string section, std::string name)
	{
		return settings[section][name].to_int();
	}

	string SettingsManager::get_string(std::string section, std::string name)
	{
		return settings[section][name].to_string();
	}

	long SettingsManager::get_long(std::string section, std::string name)
	{
		return settings[section][name].to_long();
	}

	int SettingsManager::count() const
	{
		auto count = 0;
		for(auto iterator = begin(settings); iterator != end(settings); ++iterator)
		{
			auto section_settings = (*iterator).second;
			for(auto setting_iter = begin(section_settings); setting_iter != end(section_settings); ++setting_iter)
				count++;
		}
		return count;
	}
}