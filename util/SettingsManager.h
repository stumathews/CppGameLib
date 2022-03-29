#pragma once
#include <map>
#include <string>
#include "SettingDetail.h"

namespace gamelib
{
	
	
	typedef std::map<std::string, SettingDetail>  settings;

	class SettingsManager
	{		
	public:
		static SettingsManager* Get();
		~SettingsManager();
		bool Reload();
		bool Load(std::string filename_path = "settings.xml");
		bool GetBool(std::string section, std::string name);
		int GetInt(std::string section, std::string name);
		std::string GetString(std::string section, std::string name);
		long GetLong(std::string section, std::string name);
		int Count() const;
		bool AddSetting(std::string section, std::string key, SettingDetail);
		SettingDetail GetSetting(std::string section, std::string name);
	protected:
		SettingsManager();
		static SettingsManager* Instance;
	private:
		std::map<std::string, settings> settings;
		const char* SETTINGS_SECTION = "settings";
		std::string filePath;
	};

}
