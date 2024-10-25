#include "pch.h"

#include <gtest/gtest.h>

#include "events/EventManager.h"
#include "graphic/SDLGraphicsManager.h"

using namespace std;
namespace gamelib
{
	class SceneGraphTests : public testing::Test
	{
	protected:

		void SetUp() override
		{
			
		}

		void TearDown() override {}

	};

	TEST_F(SceneGraphTests, Test1)
	{
	    std::string testAssetXml1 =
	        R"(<?xml version="1.0" encoding="utf-8"?>
				<scene id="1">
				  <layer name="player0" posx="0" posy="0" visible="true">
					<objects>	  
					 <object posx="400" posy="100" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"></object>
					</objects>
				  </layer>
				</scene>)";

		std::string testAssetXml2 =
			R"(<?xml version="1.0" encoding="utf-8"?>
				<scene id="1">
				  <layer name="NPCs" posx="0" posy="0" visible="true">

					<!-- Place a 'bicycle in the scene at 0,0 -->
	                <scene-item name="bicycle" x="0" y="0">
						<!-- Move bicycle and its children relative it its current position of 0,0 -->
						<render operation="translate" x="12" y="34"/>
						<render operation="scale" value="20pc"/> 
						<!-- Define the scene object's properties -->
						<property name="damage" value="0"/>
						<property name="type" value="bluebike"/>
						<property name="speed" value="fast"/>
						<!-- When updating, set the properties of the objects -->
						<logic>
							<if property="damage" operator="eq" value="0">
								<set property="resourceId" value="61"/>
								<else>
									<set property="resourceId" value="6"/> 
								</else>
							</if>
							<if property="damage" operator="eq" value="0">
								<raise-event type="GameObject" type="destroyed"/>
							</if>		
						<logic>
						<scene-item name="left_wheel">
							<render operation="translate" x=-10 y=0 />
							<render operation="rotate" />
							<object name="wheel" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
							<object name="spoke_frame" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
							<object name="tread" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
						</scene-item>
						<scene-item name="right_wheel">
							<render operation="translate" x=10 y=0 />
							<render operation="rotate" />
							<object name="wheel" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
							<object name="spoke_frame" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
							<object name="tread" resourceId="6" visible="true" colourKey="true" r="0" g="255" b="0"/>
						</scene-item>
					</scene-item>
				  </layer>
				</scene>)";
	}
}