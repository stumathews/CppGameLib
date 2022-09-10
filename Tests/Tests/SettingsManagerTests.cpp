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
	const auto expected_settings_count = 57;
	EXPECT_TRUE(SettingsManager::Get()->Load("settings.xml")); 

	// Should have loaded 13 settings by default from the settings file
	EXPECT_TRUE(SettingsManager::Get()->Count() == expected_settings_count);
}

TEST(SettingsManagerTests, AddSetting)
{
	const auto expected_settings_count = 60;
	SettingsManager::Get()->Load();	
	SettingsManager::Get()->AddSetting("global", "name", SettingDetail("name", "mazer", "string"));
	SettingsManager::Get()->AddSetting("global", "genre", SettingDetail("name", "platformer", "string"));
	SettingsManager::Get()->AddSetting("global", "platform", SettingDetail("name", "windows", "string"));

	// Should have added 3 extra settings
	EXPECT_TRUE(SettingsManager::Get()->Count() == expected_settings_count);
}

TEST(SettingsManagerTests, GetSetting)
{
	SettingsManager::Get()->Load();	
	SettingsManager::Get()->AddSetting("global", "name", SettingDetail("name", "mazer", "string"));
	SettingsManager::Get()->AddSetting("global", "genre", SettingDetail("name", "platformer", "string"));

	// Should be able to retrieve the settings we added
	EXPECT_EQ(SettingsManager::Get()->GetString("global", "name"), "mazer");
	EXPECT_EQ(SettingsManager::Get()->GetString("global", "genre"), "platformer");
}

TEST(SettingsManagerTests, GetSettingType)
{
	SettingsManager::Get()->Load();
	EXPECT_EQ(4, SettingsManager::Get()->GetInt("global", "max_loops"));
	EXPECT_EQ(true, SettingsManager::Get()->GetBool("global", "verbose"));
}
