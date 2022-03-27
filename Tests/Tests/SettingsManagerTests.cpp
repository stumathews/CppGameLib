#include "pch.h"
#include <util/SettingsManager.h>
using namespace gamelib;
using namespace std;

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
	const auto expected_settings_count = 32;
	EXPECT_TRUE(SettingsManager::Get()->load("settings.xml")); 

	// Should have loaded 13 settings by default from the settings file
	EXPECT_TRUE(SettingsManager::Get()->count() == expected_settings_count);
}

TEST(SettingsManagerTests, AddSetting)
{
	const auto expected_settings_count = 35;
	SettingsManager::Get()->load();	
	SettingsManager::Get()->add_setting("global", "name", setting_details("name", "mazer", "string"));
	SettingsManager::Get()->add_setting("global", "genre", setting_details("name", "platformer", "string"));
	SettingsManager::Get()->add_setting("global", "platform", setting_details("name", "windows", "string"));

	// Should have added 3 extra settings
	EXPECT_TRUE(SettingsManager::Get()->count() == expected_settings_count);
}

TEST(SettingsManagerTests, GetSetting)
{
	SettingsManager::Get()->load();	
	SettingsManager::Get()->add_setting("global", "name", setting_details("name", "mazer", "string"));
	SettingsManager::Get()->add_setting("global", "genre", setting_details("name", "platformer", "string"));

	// Should be able to retrieve the settings we added
	EXPECT_EQ(SettingsManager::Get()->get_string("global", "name"), "mazer");
	EXPECT_EQ(SettingsManager::Get()->get_string("global", "genre"), "platformer");
}

TEST(SettingsManagerTests, GetSettingType)
{
	SettingsManager::Get()->load();
	EXPECT_EQ(4, SettingsManager::Get()->get_int("global", "max_loops"));
	EXPECT_EQ("Stuart", SettingsManager::Get()->get_string("global", "author"));
	EXPECT_EQ(true, SettingsManager::Get()->get_bool("global", "verbose"));
	EXPECT_EQ(123456789L, SettingsManager::Get()->get_long("global", "longnumber"));
}
