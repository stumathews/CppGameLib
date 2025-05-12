#include "pch.h"
#include <file/SettingsManager.h>
#include <gtest/gtest.h>

using namespace std;

namespace gamelib
{
	class SettingsManagerTests : public testing::Test
	{
	protected:

		void SetUp() override
		{
			
		}

		void TearDown() override 
		{
		
		}

	};

	TEST(SettingsManagerTests, OpenFileTest)
	{
		const auto expected_settings_count = 57;
		EXPECT_TRUE(SettingsManager::Get()->ReadSettingsFile("settings.xml")); 

		// Should have loaded 13 settings by default from the settings file
		EXPECT_GT(SettingsManager::Get()->Count(),1);
	}

	TEST(SettingsManagerTests, AddSetting)
	{
		
		SettingsManager::Get()->ReadSettingsFile();	
		auto expected_settings_count = SettingsManager::Get()->Count();
		SettingsManager::Get()->SaveSetting("global", "name", SettingDetail("name", "mazer", "string"));
		SettingsManager::Get()->SaveSetting("global", "genre", SettingDetail("name", "platformer", "string"));
		SettingsManager::Get()->SaveSetting("global", "platform", SettingDetail("name", "windows", "string"));

		// Should have added 3 extra settings
		EXPECT_EQ(SettingsManager::Get()->Count(), expected_settings_count+3);
	}

	TEST(SettingsManagerTests, GetSetting)
	{
		SettingsManager::Get()->ReadSettingsFile();	
		SettingsManager::Get()->SaveSetting("global", "name", SettingDetail("name", "mazer", "string"));
		SettingsManager::Get()->SaveSetting("global", "genre", SettingDetail("name", "platformer", "string"));

		// Should be able to retrieve the settings we added
		EXPECT_EQ(SettingsManager::Get()->GetString("global", "name"), "mazer");
		EXPECT_EQ(SettingsManager::Get()->GetString("global", "genre"), "platformer");
	}

	TEST(SettingsManagerTests, GetSettingType)
	{
		SettingsManager::Get()->ReadSettingsFile();
		EXPECT_EQ(4, SettingsManager::Get()->GetInt("global", "max_loops"));
	}
}
