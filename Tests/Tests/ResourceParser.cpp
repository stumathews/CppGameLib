#include "gtest/gtest.h"
#include <graphic/GraphicAsset.h>
#include "tinyxml2.h"
#include <tuple>
#include <KeyFrame.h>
#include <Utils.h>
#include <list>

using namespace tinyxml2;
using namespace gamelib;
using namespace std;

class GraphicAssetFactory
{
public:
	GraphicAssetFactory() = default;
	GraphicAssetFactory(const GraphicAssetFactory& other) = delete;
	GraphicAssetFactory& operator=(const GraphicAssetFactory& other) = delete;
	std::shared_ptr<GraphicAsset> Create(tinyxml2::XMLElement* resourceElement);
    void ParseAnimation(tinyxml2::XMLNode* pAssetChild, std::list<gamelib::KeyFrame>& keyFrames);
    void ParseKeyFrame(tinyxml2::XMLNode* pAnimationChild, std::list<gamelib::KeyFrame>& keyFrames);
    std::map<std::string, std::string> GetNodeAttributes(XMLNode* pAssetNode);
};

std::map<std::string, std::string> GraphicAssetFactory::GetNodeAttributes(XMLNode* pAssetNode)
{
    std::map<std::string, std::string> attributes;
    if (pAssetNode)
    {

        // Nodes need to be converted to elements to access their attributes
        for (auto attribute = pAssetNode->ToElement()->FirstAttribute(); attribute; attribute = attribute->Next())
        {
            const auto* name = attribute->Name();
            const auto* value = attribute->Value();
            attributes[name] = value;
        }
    }
    return attributes;
}

std::shared_ptr<GraphicAsset> GraphicAssetFactory::Create(tinyxml2::XMLElement* assetNode)
{
    if (assetNode)
    {
        auto keyFrames = std::list<KeyFrame>();
        auto assetAttributes = GetNodeAttributes(assetNode);

        // Process known Asset children
        for (auto pAssetChild = assetNode->FirstChild(); pAssetChild; pAssetChild = pAssetChild->NextSibling())
        {
            std::string assetChildName = pAssetChild->Value();

            if (assetChildName == "animation")
            {
                ParseAnimation(pAssetChild, keyFrames);
            }
        }

        const auto uid = stoi(assetAttributes["uid"]);
        auto scene = stoi(assetAttributes["scene"]);
        auto name = assetAttributes["name"];
        auto fileName = assetAttributes["filename"];
        auto type = assetAttributes["type"];
        auto level = assetAttributes["level"] == "" ? 0 : stoi(assetAttributes["level"]);
        auto isAnimated = Utils::StrToBool(assetAttributes["isAnimated"]);

        shared_ptr<GraphicAsset> graphicAsset(new GraphicAsset(uid, name, fileName, type, level, isAnimated));
        
        // Attach key frames if there are any
        if (!keyFrames.empty())
        {
            graphicAsset->KeyFrames = keyFrames;
        }

        return graphicAsset;
    }
}

void GraphicAssetFactory::ParseAnimation(tinyxml2::XMLNode* animation, std::list<gamelib::KeyFrame>& outKeyFrames)
{
    auto animationAttributes = GetNodeAttributes(animation);

    // Process known Animation children
    for (auto pAnimationChild = animation->FirstChild(); pAnimationChild; pAnimationChild = pAnimationChild->NextSibling())
    {
        std::string animationChildName = pAnimationChild->Value();

        if (animationChildName == "keyframe")
        {
            ParseKeyFrame(pAnimationChild, outKeyFrames);
        }
    }
}

void GraphicAssetFactory::ParseKeyFrame(tinyxml2::XMLNode* pAnimationChild, std::list<gamelib::KeyFrame>& outKeyFrames)
{
    auto keyFrameAttributes = GetNodeAttributes(pAnimationChild);
    auto x = stoi(keyFrameAttributes.at("x"));
    auto y = stoi(keyFrameAttributes.at("y"));
    auto w = stoi(keyFrameAttributes.at("w"));
    auto h = stoi(keyFrameAttributes.at("h"));

    outKeyFrames.push_back(KeyFrame(x, y, w, h));
}


TEST(ResourceParserTests, TestName) 
{
    std::string testAssetXml = 
        R"(<Asset uid="9" scene="3" name="p1.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png" 
                  isAnimated="true" numKeyFrames="11" keyFrameHeight="97" keyFrameWidth="73">    
    	    <animation>
                <keyframe x="0" y="0" w="66" h="92"/>
                <keyframe x="66" y="0" w="66" h="93"/>
                <keyframe x="132" y="0" w="66" h="93"/>
                <keyframe x="0" y="93" w="66" h="93"/>
                <keyframe x="66" y="93" w="66" h="93"/>
                <keyframe x="132" y="93" w="71" h="92"/>
                <keyframe x="0" y="186" w="71" h="93"/>
                <keyframe x="71" y="186" w="71" h="93"/>
                <keyframe x="142" y="186" w="70" h="92"/>
                <keyframe x="0" y="279" w="71" h="93"/>
                <keyframe x="71" y="279" w="66" h="97"/>
          </animation>   
        </Asset>)";

    GraphicAssetFactory graphicAssetFactory;

    XMLDocument doc;
    doc.Parse(testAssetXml.c_str());

    if (doc.ErrorID() == 0)
    {
        // Get Asset element
        XMLElement* pAssetNode = doc.FirstChildElement("Asset");
        auto asset = graphicAssetFactory.Create(pAssetNode);
        EXPECT_EQ(asset->KeyFrames.size(), 11);
    }
}

