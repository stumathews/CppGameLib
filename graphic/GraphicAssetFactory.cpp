#include "GraphicAssetFactory.h"


using namespace tinyxml2;
using namespace std;

namespace gamelib
{
    map<string, string> GraphicAssetFactory::GetNodeAttributes(XMLNode* pAssetNode)
    {
        map<string, string> attributes;
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

    GraphicAssetFactory* GraphicAssetFactory::Get()
    {
        if (Instance == nullptr)
        {
            Instance = new GraphicAssetFactory();
        }
        return Instance;
    }

    GraphicAssetFactory::~GraphicAssetFactory()
    {
        Instance = nullptr;
    }

    GraphicAssetFactory* GraphicAssetFactory::Instance = nullptr;

    shared_ptr<Asset> GraphicAssetFactory::Parse(tinyxml2::XMLElement* assetNode)
    {
        // We will be returning a GraphicAsset upcasted to an Asset
        shared_ptr<Asset> asset = nullptr;

        if (assetNode)
        {
            // Get Asset attributes
            auto assetAttributes = GetNodeAttributes(assetNode);

            // Extract asset attributes
            const auto uid = stoi(assetAttributes["uid"]);
            auto name = assetAttributes["name"];
            auto fileName = assetAttributes["filename"];
            auto type = assetAttributes["type"];
            auto level = assetAttributes["scene"] == "" ? 0 : stoi(assetAttributes["scene"]);
            auto isAnimated = Utils::StrToBool(assetAttributes["isAnimated"]);
            auto width = stoi(assetAttributes["width"]);
            auto height = stoi(assetAttributes["height"]);
            auto dimensions = ABCDRectangle(0, 0, width, height);
            
            // Process Asset children
            for (auto pAssetChild = assetNode->FirstChild(); pAssetChild; pAssetChild = pAssetChild->NextSibling())
            {
                string assetChildName = pAssetChild->Value();
                
                // Look to see if this is a sprite
                shared_ptr<SpriteAsset> sprite = nullptr;
                if (assetChildName == "sprite")
                {
                    // Yes, is a sprite, process it
                    sprite = shared_ptr<SpriteAsset>(new SpriteAsset(uid, name, fileName, type, level, dimensions));
                    
                    ParseSprite(pAssetChild, sprite);

                    // Upcast to Asset
                    asset = sprite;
                    asset->assetType = Asset::AssetType::Sprite;
                }                
            }

            // If asset is not a specialization of Graphic Asset, its just a plain graphic Asset
            if (asset == nullptr)
            {
                asset = shared_ptr<GraphicAsset>(new GraphicAsset(uid, name, fileName, type, level, dimensions));
                asset->assetType = Asset::AssetType::Graphic;
            }
        }
        return asset;
    }

    void GraphicAssetFactory::ParseSpriteAnimation(tinyxml2::XMLNode* animation, shared_ptr<SpriteAsset> sprite)
    {
        auto animationAttributes = GetNodeAttributes(animation);

        // Process known Animation children
        for (auto pAnimationChild = animation->FirstChild(); pAnimationChild; pAnimationChild = pAnimationChild->NextSibling())
        {
            string animationChildName = pAnimationChild->Value();

            if (animationChildName == "keyframes")
            {
                ParseSpriteKeyFrames(pAnimationChild, sprite);
            }
        }
    }

    void GraphicAssetFactory::ParseSprite(tinyxml2::XMLNode* pSprite, shared_ptr<SpriteAsset> sprite)
    {
        for (auto pSpriteChild = pSprite->FirstChild(); pSpriteChild; pSpriteChild = pSpriteChild->NextSibling())
        {
            string spriteChildName = pSpriteChild->Value();

            if (spriteChildName == "animation")
            {
                auto attributes = GetNodeAttributes(pSpriteChild);
                ParseSpriteAnimation(pSpriteChild, sprite);
            }
        }        
    }

    void GraphicAssetFactory::ParseSpriteKeyFrames(tinyxml2::XMLNode* pKeyFrames, shared_ptr<SpriteAsset> sprite)
    {
        for (auto pKeyFrameChild = pKeyFrames->FirstChild(); pKeyFrameChild; pKeyFrameChild = pKeyFrameChild->NextSibling())
        {
            string spriteChildName = pKeyFrameChild->Value();

            if (spriteChildName == "keyframe")
            {
                ParseSpriteKeyFrame(pKeyFrameChild, sprite);
            }
        }
    }

    void GraphicAssetFactory::ParseSpriteKeyFrame(tinyxml2::XMLNode* pKeyFrame, shared_ptr<SpriteAsset> sprite)
    {
        auto keyFrameAttributes = GetNodeAttributes(pKeyFrame);
        auto x = stoi(keyFrameAttributes.at("x"));
        auto y = stoi(keyFrameAttributes.at("y"));
        auto w = stoi(keyFrameAttributes.at("w"));
        auto h = stoi(keyFrameAttributes.at("h"));

        sprite->KeyFrames.push_back(KeyFrame(x, y, w, h));
    }
}

