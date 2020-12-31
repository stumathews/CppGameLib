#include "pch.h"
#include <util/settings_manager.h>
using namespace gamelib;
using namespace std;

TEST(SettingsManager, OpenFileTest)
{
	const auto expected_settings_count = 32;
	settings_manager settings;
	EXPECT_TRUE(settings.load("settings.xml")); 

	// Should have loaded 13 settings by default from the settings file
	EXPECT_TRUE(settings.count() == expected_settings_count);
}

TEST(SettingsManager, AddSetting)
{
	const auto expected_settings_count = 35;
	settings_manager settings;
	settings.load();	
	settings.add_setting("global", "name", setting_details("name", "mazer", "string"));
	settings.add_setting("global", "genre", setting_details("name", "platformer", "string"));
	settings.add_setting("global", "platform", setting_details("name", "windows", "string"));

	// Should have added 3 extra settings
	EXPECT_TRUE(settings.count() == expected_settings_count);
}

TEST(SettingsManager, GetSetting)
{
	settings_manager settings;
	settings.load();	
	settings.add_setting("global", "name", setting_details("name", "mazer", "string"));
	settings.add_setting("global", "genre", setting_details("name", "platformer", "string"));

	// Should be able to retrieve the settings we added
	EXPECT_EQ(settings.get_string("global", "name"), "mazer");
	EXPECT_EQ(settings.get_string("global", "genre"), "platformer");
}

TEST(SettingsManager, GetSettingType)
{
	settings_manager settings;
	settings.load();
	EXPECT_EQ(4, settings.get_int("global", "max_loops"));
	EXPECT_EQ("Stuart", settings.get_string("global", "author"));
	EXPECT_EQ(true, settings.get_bool("global", "verbose"));
	EXPECT_EQ(123456789L, settings.get_long("global", "longnumber"));
}
