#include "pch.h"
#include <util/SettingsManager.h>
using namespace gamelib;
using namespace std;

TEST(SettingsManagerTests, OpenFileTest)
{
	const auto expected_settings_count = 32;
	SettingsManager settings;
	EXPECT_TRUE(settings.load("settings.xml")); 

	// Should have loaded 13 settings by default from the settings file
	EXPECT_TRUE(settings.count() == expected_settings_count);
}

TEST(SettingsManagerTests, AddSetting)
{
	const auto expected_settings_count = 35;
	SettingsManager settings;
	settings.load();	
	settings.add_setting("global", "name", setting_details("name", "mazer", "string"));
	settings.add_setting("global", "genre", setting_details("name", "platformer", "string"));
	settings.add_setting("global", "platform", setting_details("name", "windows", "string"));

	// Should have added 3 extra settings
	EXPECT_TRUE(settings.count() == expected_settings_count);
}

TEST(SettingsManagerTests, GetSetting)
{
	SettingsManager settings;
	settings.load();	
	settings.add_setting("global", "name", setting_details("name", "mazer", "string"));
	settings.add_setting("global", "genre", setting_details("name", "platformer", "string"));

	// Should be able to retrieve the settings we added
	EXPECT_EQ(settings.get_string("global", "name"), "mazer");
	EXPECT_EQ(settings.get_string("global", "genre"), "platformer");
}

TEST(SettingsManagerTests, GetSettingType)
{
	SettingsManager settings;
	settings.load();
	EXPECT_EQ(4, settings.get_int("global", "max_loops"));
	EXPECT_EQ("Stuart", settings.get_string("global", "author"));
	EXPECT_EQ(true, settings.get_bool("global", "verbose"));
	EXPECT_EQ(123456789L, settings.get_long("global", "longnumber"));
}
