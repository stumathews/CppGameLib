#include "SettingsManager.h"
#include "file/tinyxml2.h"
#include <exceptions/EngineException.h>
using namespace std;

namespace gamelib
{
	bool SettingsManager::Reload()
	{
		return ReadSettingsFile(filePath);
	}

	SettingsManager::SettingsManager() 
	{
		ReadSettingsFile(); 
	}

	tinyxml2::XMLComment* SettingsManager::IsComment(tinyxml2::XMLNode* section)
	{
		return section->ToComment();
	}

	bool SettingsManager::ReadSettingsFile(const string& filenamePath)
	{
		this->filePath = filenamePath;

		//reload the settings from scratch each time
		settings.clear();

		// Parse the settings XML file:
		
		/* Example Settings file:
		 * ----------------------
		 
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

		tinyxml2::XMLDocument doc;
		
		if(doc.LoadFile(filenamePath.c_str()) == tinyxml2::XML_SUCCESS)
		{
			// Loop through settings section
			for(auto section = doc.FirstChildElement(settingsSection)->FirstChild(); section; section = section->NextSibling())
			{
				// Ignore comments
				if (IsComment(section)) 
				{
					continue;
				}

				// Loop through each setting section e.g., 'global' or 'player' etc.
				const auto* sectionName = section->Value();

				for(auto* setting = section->FirstChild(); setting; setting = setting->NextSibling())
				{
					if (IsComment(setting))
					{
						continue;
					}

					// Loop through the settings attributes i.e., name, type etc.
					auto settingElement = setting->ToElement();
					string settingName;
					string settingValue;
					string settingType;

					for(const auto* attributes = settingElement->FirstAttribute(); attributes; attributes = attributes->Next())
					{
						const string key(attributes->Name());
						const string value(attributes->Value());						

						if (key == "name")
						{
							settingName = value;
						}

						if (key == "type")
						{
							settingType = value;
						}
					}

					// Persist the setting into the SettingsManager
					SaveSetting(sectionName, settingName,  SettingDetail(settingName, settingElement->GetText(), settingType));
				}
			}
		}
		else
		{
			// XML file parse failed
			return false;
		}

		// XML file parse succeeded
		return true;
	}

	bool SettingsManager::SaveSetting(const std::string& section, const std::string& key, const SettingDetail& details)
	{
		try
		{
			// Save the setting in memory
			settings[section][key] = details;
		}
		catch ([[maybe_unused]] exception& e)
		{
			// Could not save the setting for some reason
			return false;
		}

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
			auto sectionSettings = (*iterator).second;
			for (auto settingIter = begin(sectionSettings); settingIter != end(sectionSettings); ++settingIter) 
			{
				count++;
			}
		}
		return count;
	}

	SettingsManager* SettingsManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new SettingsManager();
		}
		return instance;
	}

	SettingsManager::~SettingsManager()
	{
		instance = nullptr;
	}

	SettingsManager* SettingsManager::instance = nullptr;
}