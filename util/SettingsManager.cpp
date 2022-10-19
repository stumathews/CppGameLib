#include "SettingsManager.h"
#include "tinyxml2.h"
#include <exceptions/EngineException.h>
using namespace std;

namespace gamelib
{
	

	bool SettingsManager::Reload()
	{
		return Load(filePath);
	}

	SettingsManager::SettingsManager() 
	{ 
		Load(); 
	}

	SettingsManager* SettingsManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new SettingsManager();
		}
		return Instance;
	}

	SettingsManager::~SettingsManager()
	{
		Instance = nullptr;
	}

	SettingsManager* SettingsManager::Instance = nullptr;

	bool SettingsManager::Load(string filePath)
	{
		this->filePath = filePath;
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

		
		if(doc.LoadFile(filePath.c_str()) == tinyxml2::XML_SUCCESS)
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
					AddSetting(section_name, setting_name,  SettingDetail(setting_name, setting_element->GetText(), setting_type));
				}
			}
		}
		else
		{
			return false;
		}
		return true;
	}

	bool SettingsManager::AddSetting(std::string section, std::string key, SettingDetail details)
	{
		settings[section][key] = details;
		return true;
	}

	SettingDetail SettingsManager::GetSetting(std::string section, std::string name)
	{
		return settings[section][name];
	}
	
	bool SettingsManager::GetBool(std::string section, std::string name)
	{
		return settings.at(section)[name].ToBool();		
	}
	
	int SettingsManager::GetInt(std::string section, std::string name)
	{
		try
		{
			return settings[section][name].ToInt();
		}
		catch (exception& e)
		{
			THROW(12, "Could not find setting [" + section + "][" + name + "]", "Settings");
		}
	}

	string SettingsManager::GetString(std::string section, std::string name)
	{
		return settings[section][name].ToString();
	}

	long SettingsManager::GetLong(std::string section, std::string name)
	{
		return settings[section][name].ToLong();
	}

	int SettingsManager::Count() const
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