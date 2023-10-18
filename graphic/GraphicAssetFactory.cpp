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

    shared_ptr<Asset> GraphicAssetFactory::Parse(XMLElement* assetNode) const
    {
        // We will be returning a GraphicAsset up-cast to an Asset
        shared_ptr<GraphicAsset> graphicAsset = nullptr;

        if (assetNode)
        {
            // Get Asset attributes
            auto assetAttributes = GetNodeAttributes(assetNode);

            // Extract asset attributes
            auto uid = stoi(assetAttributes["uid"]);
            auto name = assetAttributes["name"];
            auto fileName = assetAttributes["filename"];
            auto type = assetAttributes["type"];
            auto level = assetAttributes["scene"].empty() ? 0 : stoi(assetAttributes["scene"]);
            const auto width = stoi(assetAttributes["width"]);
            const auto height = stoi(assetAttributes["height"]);
            auto dimensions = ABCDRectangle(0, 0, width, height);

            graphicAsset = std::make_shared<GraphicAsset>(uid, name, fileName, type, level, dimensions);
            ColourKey ColourKey;

            // Process Asset children
            for (auto pAssetChild = assetNode->FirstChild(); pAssetChild; pAssetChild = pAssetChild->NextSibling())
            {
                string assetChildName = pAssetChild->Value();
                
                // Look to see if this is a sprite
                shared_ptr<SpriteAsset> _sprite = nullptr;
                if (assetChildName == "sprite")
                {
                    // Yes, is a sprite, process it
                    _sprite = std::make_shared<SpriteAsset>(graphicAsset->Uid, graphicAsset->Name, graphicAsset->FilePath,
                                                            graphicAsset->type, graphicAsset->SceneId, dimensions);
                    
                    ParseSprite(pAssetChild, _sprite);

                    // Upcast to Asset
                    graphicAsset = _sprite;
                    graphicAsset->AssetType = Asset::AssetType::Sprite;
                }

                if (assetChildName == "colorkey")
                {
                    auto attributes = GetNodeAttributes(pAssetChild);
                    const auto red = stoi(attributes.at("red"));
                    const auto green = stoi(attributes.at("green"));
                    const auto blue = stoi(attributes.at("blue"));
                    ColourKey = gamelib::ColourKey(red, green, blue);
                }
            }

            if (ColourKey.IsSet())
            {
                graphicAsset->SetColourKey(ColourKey.Red, ColourKey.Green, ColourKey.Blue);
            }
        }
        return graphicAsset;
    }

    void GraphicAssetFactory::ParseSpriteAnimation(XMLNode* animation, const shared_ptr<SpriteAsset>& sprite) const
    {
        auto animationAttributes = GetNodeAttributes(animation);

        // Process known Animation children
        for (auto pAnimationChild = animation->FirstChild(); pAnimationChild; pAnimationChild = pAnimationChild->NextSibling())
        {
            string animationChildName = pAnimationChild->Value();

            if (animationChildName == "keyframes")
            {
                auto attributes = GetNodeAttributes(pAnimationChild);
                float duration = 0;
                if (attributes.count("duration") > 0)
                {
                    duration = stof(attributes.at("duration"));
                }
                sprite->FrameDurationMs = duration;
                ParseSpriteKeyFrames(pAnimationChild, sprite);
            }
        }
    }

    void GraphicAssetFactory::ParseSprite(XMLNode* pSprite, const shared_ptr<SpriteAsset>& sprite) const
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

    void GraphicAssetFactory::ParseSpriteKeyFrames(XMLNode* pKeyFrames, const shared_ptr<SpriteAsset>& sprite) const
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

    void GraphicAssetFactory::ParseSpriteKeyFrame(XMLNode* pKeyFrame, const shared_ptr<SpriteAsset>& sprite) const
    {
        auto keyFrameAttributes = GetNodeAttributes(pKeyFrame);
        const auto x = stoi(keyFrameAttributes.at("x"));
        const auto y = stoi(keyFrameAttributes.at("y"));
        const auto w = stoi(keyFrameAttributes.at("w"));
        const auto h = stoi(keyFrameAttributes.at("h"));
        const auto hasGroup = keyFrameAttributes.count("group") > 0;
        const string group = hasGroup ? keyFrameAttributes["group"] : "";
        
        sprite->KeyFrames.emplace_back(x, y, w, h, group);
    }
}

