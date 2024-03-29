#include "SettingsManager.h"
#include "file/tinyxml2.h"
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

	bool SettingsManager::Load(const string& filenamePath)
	{
		this->filePath = filenamePath;
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

		
		if(doc.LoadFile(filenamePath.c_str()) == tinyxml2::XML_SUCCESS)
		{
			// Loop through settings section
			for(auto section = doc.FirstChildElement(SETTINGS_SECTION)->FirstChild(); section; section = section->NextSibling())
			{	
				if(section->ToComment())
					continue;

				// Loop through each setting section eg. 'global' or 'player' etc.
				const auto* sectionName = section->Value();
				for(auto* setting = section->FirstChild(); setting; setting = setting->NextSibling())
				{
					if(setting->ToComment())
					continue;

					// Loop through the settings attributes i.e name, type etc.
					auto settingElement = setting->ToElement();
					string settingName;
					string settingValue;
					string settingType;

					for(const auto* attributes = settingElement->FirstAttribute(); attributes; attributes = attributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
					{
						const string key(attributes->Name());
						const string value(attributes->Value());						

						if(key == "name")
							settingName = value;
						if(key == "type")
							settingType = value;
					}

					// Persist the setting into the SettingsManager
					AddSetting(sectionName, settingName,  SettingDetail(settingName, settingElement->GetText(), settingType));
				}
			}
		}
		else
		{
			return false;
		}
		return true;
	}

	bool SettingsManager::AddSetting(const std::string& section, const std::string& key, const SettingDetail& details)
	{
		settings[section][key] = details;
		return true;
	}

	SettingDetail SettingsManager::GetSetting(const std::string& section, const std::string& name)
	{
		return settings[section][name];
	}
	
	bool SettingsManager::GetBool(const std::string& section, const std::string& name)
	{
		return settings.at(section)[name].ToBool();		
	}

	bool SettingsManager::Bool(const std::string& section, const std::string& name)
	{
		return Get()->GetBool(section, name);
	}
	
	int SettingsManager::GetInt(const std::string& section, const std::string& name)
	{
		try
		{
			return settings[section][name].ToInt();
		}
		catch (exception&)
		{
			THROW(12, "Could not find setting [" + section + "][" + name + "]", "Settings");
		}
	}

	int SettingsManager::Int(const std::string& section, const std::string& name)
	{
		return Get()->GetInt(section, name);
	}

	string SettingsManager::GetString(const std::string& section, const std::string& name)
	{
		return settings[section][name].ToString();
	}

	string SettingsManager::String(const std::string& section, const std::string& name)
	{
		return Get()->GetString(section, name);
	}

	long SettingsManager::GetLong(const std::string& section, const std::string& name)
	{
		return settings[section][name].ToLong();
	}

	int SettingsManager::Count() const
	{
		auto count = 0;
		for(auto iterator = begin(settings); iterator != end(settings); ++iterator)
		{
			auto section_settings = (*iterator).second;
			for(auto settingIter = begin(section_settings); settingIter != end(section_settings); ++settingIter)
				count++;
		}
		return count;
	}
}