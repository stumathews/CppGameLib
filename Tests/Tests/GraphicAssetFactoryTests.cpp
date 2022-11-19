//#include "gtest/gtest.h"
#include "pch.h"
#include <graphic/GraphicAsset.h>
#include "tinyxml2.h"
#include <tuple>
#include <KeyFrame.h>
#include <Utils.h>
#include <list>
#include <graphic/GraphicAssetFactory.h>

using namespace tinyxml2;
using namespace std;
using namespace gamelib;

TEST(GraphicAssetFactoryTests, ParseStaticSprite)
{
    std::string testAssetXml = R"(<Asset uid="9" scene="3" name="p1.png" type="graphic" width="64" height="65" filename="Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png"/>)";

    // Load XML
    XMLDocument doc;
    doc.Parse(testAssetXml.c_str());
    EXPECT_EQ(doc.ErrorID(), 0);

    auto node = doc.FirstChildElement("Asset");

    // When Parsing the node
    auto asset = GraphicAssetFactory::Get()->Parse(node);
    auto graphicAsset = dynamic_pointer_cast<GraphicAsset>(asset);
    // Ensure...
    EXPECT_EQ(graphicAsset->uid, 9);
    EXPECT_EQ(graphicAsset->scene, 3);
    EXPECT_EQ(graphicAsset->Dimensions.GetWidth(), 64);
    EXPECT_EQ(graphicAsset->Dimensions.GetHeight(), 65);
    EXPECT_EQ(graphicAsset->name, string("p1.png"));
    EXPECT_EQ(graphicAsset->type, string("graphic"));
    EXPECT_EQ(graphicAsset->path, string("Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png"));
    EXPECT_EQ(graphicAsset->assetType, gamelib::Asset::AssetType::Graphic);
    EXPECT_EQ(graphicAsset->KeyFrames.size(), 0);

}

TEST(GraphicAssetFactoryTests, ParseSprite)
{
    std::string testAssetXml =
        R"(<Asset uid="9" scene="3" name="p1.png" type="graphic" width="64" height="65" filename="Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png">    
            <colorkey red="255" green="255" blue="255"/>
    	    <sprite>
                <animation>
                    <keyframes>
                        <keyframe x="0" y="0" w="66" h="92"/>
                        <keyframe x="66" y="0" w="66" h="93"/>
                        <keyframe x="132" y="0" w="66" h="93"/>
                        <keyframe x="0" y="93" w="66" h="93"/>
                        <keyframe x="66" y="93" w="66" h="93" group="one" />
                        <keyframe x="132" y="93" w="71" h="92" group="one" />
                        <keyframe x="0" y="186" w="71" h="93"/>
                        <keyframe x="71" y="186" w="71" h="93"/>
                        <keyframe x="142" y="186" w="70" h="92"/>
                        <keyframe x="0" y="279" w="71" h="93"/>
                        <keyframe x="71" y="279" w="66" h="97" group="one"/>
                    </keyframes>
              </animation> 
          </sprite>  
        </Asset>)";

    // Load XML
    XMLDocument doc;
    doc.Parse(testAssetXml.c_str());
    EXPECT_EQ(doc.ErrorID(), 0);

    auto node = doc.FirstChildElement("Asset");

    // When Parsing the node
    auto asset = GraphicAssetFactory::Get()->Parse(node);
    auto _sprite = dynamic_pointer_cast<SpriteAsset>(asset);
    // Ensure...
    EXPECT_EQ(_sprite->uid, 9);
    EXPECT_EQ(_sprite->scene, 3);
    EXPECT_EQ(_sprite->Dimensions.GetWidth(), 64);
    EXPECT_EQ(_sprite->Dimensions.GetHeight(), 65);
    EXPECT_EQ(_sprite->name, string("p1.png"));
    EXPECT_EQ(_sprite->type, string("graphic"));
    EXPECT_EQ(_sprite->path, string("Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png"));
    EXPECT_EQ(_sprite->assetType, gamelib::Asset::AssetType::Sprite);
    EXPECT_EQ(_sprite->KeyFrames.size(), 11);
    EXPECT_EQ(_sprite->GetColourKey(), ColourKey(255,255,255)) << "Invalid colour key";
    EXPECT_TRUE(_sprite->KeyFrames[4].HasGroup());
    EXPECT_EQ(_sprite->KeyFrames[4].group, "one");
    EXPECT_TRUE(_sprite->KeyFrames[5].HasGroup());
    EXPECT_EQ(_sprite->KeyFrames[5].group, "one");
    EXPECT_TRUE(_sprite->KeyFrames[10].HasGroup());
    EXPECT_EQ(_sprite->KeyFrames[10].group, "one");    
    EXPECT_FALSE(_sprite->KeyFrames[0].HasGroup());
}

