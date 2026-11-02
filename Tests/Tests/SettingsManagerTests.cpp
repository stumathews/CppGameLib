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
		constexpr auto expected_settings_count = 57;
		EXPECT_TRUE(SettingsManager::Get()->ReadSettingsFile("settings.xml")); 

		// Should have loaded 13 settings by default from the settings file
		EXPECT_GT(SettingsManager::Get()->Count(),1);
	}

	TEST(SettingsManagerTests, AddSetting)
	{
		
		SettingsManager::Get()->ReadSettingsFile();
		const auto expected_settings_count = SettingsManager::Get()->Count();
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

	TEST(SettingsManagerTests, ReadArray)
	{
		SettingsManager::Get()->ReadSettingsFile();
		/*
		<test>
			<setting name="numbers" type="int[]">
				<item>1</item>
				<item>2</item>
				<item>3</item>
			</setting>
			<setting name="names" type="string[]">
				<item>Stuart</item>
				<item>Bruce</item>
				<item>Jenny</item>
			</setting>
		</test>
		 */
		auto numbers =  SettingsManager::Get()->GetIntArray("test", "numbers");
		EXPECT_EQ(numbers.size(), 3);
		EXPECT_EQ(numbers[0], 1);
		EXPECT_EQ(numbers[1], 2);
		EXPECT_EQ(numbers[2], 3);
		auto names =  SettingsManager::Get()->GetStringArray("test", "names");
		EXPECT_EQ(numbers.size(), 3);
		EXPECT_STREQ(names[0].c_str(), "Stuart");
		EXPECT_STREQ(names[1].c_str(), "Bruce");
		EXPECT_STREQ(names[2].c_str(), "Jenny");

		/*
			<gamecommands>
				<setting name="logCommands" type="bool">false</setting>
				<setting name="types" type="string[]">
					<item>Info</item>
					<item>Warning</item>
					<item>Error</item>
				</setting>
			</gamecommands>
		 */

		auto logTypes =  SettingsManager::Get()->GetStringArray("gamecommands", "types");
		EXPECT_EQ(logTypes.size(), 3);
		EXPECT_STREQ(logTypes[0].c_str(), "Info");
		EXPECT_STREQ(logTypes[1].c_str(), "Warning");
		EXPECT_STREQ(logTypes[2].c_str(), "Error");
	}
}
