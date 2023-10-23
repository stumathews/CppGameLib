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
		bool Load(const std::string& filenamePath = "settings.xml");
		bool GetBool(const std::string& section, const std::string& name);
		static bool Bool(const std::string& section, const std::string& name);		
		static std::string String(const std::string& section, const std::string& name);
		int GetInt(const std::string& section, const std::string& name);
		static int Int(const std::string& section, const std::string& name);
		std::string GetString(const std::string& section, const std::string& name);
		long GetLong(const std::string& section, const std::string& name);
		[[nodiscard]] int Count() const;
		bool AddSetting(const std::string& section, const std::string& key, const SettingDetail&);
		SettingDetail GetSetting(const std::string& section, const std::string& name);
	protected:
		SettingsManager();
		static SettingsManager* Instance;
	private:
		std::map<std::string, settings> settings;
		const char* SETTINGS_SECTION = "settings";
		std::string filePath;
	};

}
