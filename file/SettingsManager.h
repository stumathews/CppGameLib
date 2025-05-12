#pragma once
#include <map>
#include <string>
#include "SettingDetail.h"
#include "tinyxml2.h"

class LevelManager;

namespace gamelib
{
	typedef std::map<std::string, SettingDetail>  InternalSettings;

	class SettingsManager
	{		
	public:
		static SettingsManager* Get();
		~SettingsManager();
		SettingsManager& operator=(const SettingsManager& other) = delete;
		SettingsManager& operator=(const SettingsManager&& other) = delete;
		SettingsManager(const SettingsManager& other) = delete;
		SettingsManager(const SettingsManager&& other) = delete;

		[[nodiscard]] int Count() const;

		bool ReadSettingsFile(const std::string& filenamePath = "settings.xml");
		bool Reload();

		bool SaveSetting(const std::string& section, const std::string& key, const SettingDetail&);
		SettingDetail GetSetting(const std::string& section, const std::string& name);

		// Integer settings
		int GetInt(const std::string& section, const std::string& name);
		static int Int(const std::string& section, const std::string& name);

		// Long settings
		long GetLong(const std::string& section, const std::string& name);

		// Boolean settings
		static bool Bool(const std::string& section, const std::string& name);
		bool GetBool(const std::string& section, const std::string& name);

		// String settings
		static std::string String(const std::string& section, const std::string& name);
		std::string GetString(const std::string& section, const std::string& name);
	protected:
		SettingsManager();
		static SettingsManager* instance;
	private:
		static tinyxml2::XMLComment* IsComment(tinyxml2::XMLNode* section);
		std::map<std::string, InternalSettings> settings;
		const char* settingsSection = "settings";
		std::string filePath;
	};
}
