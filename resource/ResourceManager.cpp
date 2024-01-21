#include "ResourceManager.h"
#include "file/tinyxml2.h"
#include <map>
#include <memory>
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "events/EventManager.h"
#include "events/SceneChangedEvent.h"
#include "font/FontManager.h"
#include <exceptions/EngineException.h>
#include "graphic/GraphicAssetFactory.h"
#include "file/SettingsManager.h"

namespace gamelib
{
	using namespace tinyxml2;
	using namespace std;

	ResourceManager::ResourceManager(): debug(false) {  }
	ResourceManager::~ResourceManager() { Instance = nullptr; }

	/// <summary>
	/// Initialise the resource manager
	/// </summary>
	bool ResourceManager::Initialize(const std::string& filePath)
	{
		IndexResourceFile(filePath);
		debug = SettingsManager::Get()->GetBool("global", "verbose");
		return LogThis("ResourceManager::initialize()", debug, [&]()
		{
			EventManager::Get()->SubscribeToEvent(LevelChangedEventTypeEventId, this);
			// we will load the resources for the level that has been loaded
			return true;
		}, true, true);
	}

	vector<shared_ptr<Event>> ResourceManager::HandleEvent(const std::shared_ptr<Event>& event, const unsigned long deltaMs)
	{
		if (event->Id.PrimaryId == LevelChangedEventTypeEventId.PrimaryId)
		{
			LogThis("Detected level change. Loading level assets...", debug, [&]()
			{
				LoadSceneAssets(dynamic_pointer_cast<SceneChangedEvent>(event)->SceneId);
				return true;
			}, true, true);
		}

		return {};
	}

	/// <summary>
	/// Load all the resources required by the scene and unload out all those that are not in the scene.
	/// </summary>
	/// <remarks>Other scene assets are unloaded</remarks>
	/// <param name="level">Scene/Level assets to load.</param>
	void ResourceManager::LoadSceneAssets(const int level)
	{
		for (const auto& levelResources : resourcesByScene) // we need access to all resources to swap in/out resources
		{
			for (const auto& asset : levelResources.second)
			{
				const auto alwaysLoadResource = asset->SceneId == 0;
				if ((asset->SceneId == level || alwaysLoadResource) && !asset->IsLoadedInMemory)
				{
					asset->Load();

					Logger::Get()->LogThis(
						string("scene " + to_string(asset->SceneId)) + string(": ") + string(asset->Name) +
						" asset loaded.");

					countLoadedResources++;
					countUnloadedResources--;
				}
				else if (asset->IsLoadedInMemory && asset->SceneId != level && !alwaysLoadResource)
				{
					asset->Unload();

					Logger::Get()->LogThis(
						string("scene: " + to_string(asset->SceneId)) + string(asset->Name) + " asset unloaded.");
					countUnloadedResources++;
					countLoadedResources--;
				}
			}
		}
	}

	/// <summary>
	/// Ask each asset to unload itself
	/// </summary>
	void ResourceManager::Unload() const
	{
		LogThis("Unloading all resources...", debug, [&]()
		{
			for (const auto& item : resourcesByName)
			{
				auto& assetName = item.first;
				auto& asset = item.second;

				asset->Unload();

				LogMessage("Unloaded asset '" + assetName + string("'."));
			}
			return true;
		}, true, true);
	}

	/// <summary>
	/// Index the Resources.xml file	 
	/// </summary>
	void ResourceManager::IndexResourceFile(const string& resourcesFilePath)
	{
		Logger::Get()->LogThis("ResourceManager: reading resources.xml.");

		XMLDocument xmlDocument;
		xmlDocument.LoadFile(resourcesFilePath.c_str());

		if (xmlDocument.ErrorID() == 0)
		{
			auto* pAssetsNode = xmlDocument.FirstChildElement("Assets");

			if (pAssetsNode)
			{
				for (auto child = pAssetsNode->FirstChild(); child; child = child->NextSibling())
				{
					auto* const element = child->ToElement();
					if (element)
					{
						shared_ptr<Asset> theAsset = nullptr;
						const char* ptrAssetType;

						element->QueryStringAttribute("type", &ptrAssetType);
						theAsset = CreateAssetFromElement(ptrAssetType, theAsset, element);

						// Store the asset reference, but don't load it
						if (theAsset != nullptr)
						{
							StoreAsset(theAsset);
						}
						else
						{
							const auto message = string("No asset manager defined for ") + ptrAssetType;
							Logger::Get()->LogThis(message);
							THROW(static_cast<int>(ResourceManager::ErrorNumbers::NoAssetManagerForType), message,
							      this->GetSubscriberName());
						}
					}
				}
			}
			else
			{
				THROW(12, "No Assets found", GetSubscriberName());
			}
		}
		else
		{
			THROW(static_cast<int>(ResourceManager::ErrorNumbers::FailedToLoadResourceFile),
			      "Failed to load resources file", this->GetSubscriberName());
		}

		LogMessage(to_string(countResources) + string(" assets available in resource manager."));
	}

	/// <summary>
	/// Creates Asset Info from an entry in the resource file
	/// </summary>
	/// <param name="type">Type of asset identified in the resource file</param>
	/// <param name="asset"></param>
	/// <param name="element">the current element containing the asset data</param>
	/// <returns></returns>
	std::shared_ptr<Asset>& ResourceManager::CreateAssetFromElement(const char* type, std::shared_ptr<Asset>& asset,
	                                                                XMLElement* const& element)
	{
		// Create a graphic asset
		if (strcmp(type, "graphic") == 0)
		{
			asset = GraphicAssetFactory::Get()->Parse(element);
		}

		// Create fx/sound/audio asset
		else if (strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
		{
			asset = AudioManager::Get()->CreateAsset(element, *this);
		}

		// Create font asset
		else if (strcmp(type, "font") == 0)
		{
			asset = FontManager::Get()->CreateAsset(element);
		}
		else
		{
			const auto message = string("Unknown resource type:") + type;
			THROW(static_cast<int>(ResourceManager::ErrorNumbers::UnknownResourceType), message, GetSubscriberName());
		}

		return asset;
	}

	/// <summary>
	/// Indexes the asset info
	/// </summary>
	/// <param name="asset">the asset info to store</param>
	void ResourceManager::StoreAsset(const shared_ptr<Asset>& asset)
	{
		// assets are explicitly associated with a scene that it will work in
		resourcesByScene[asset->SceneId].push_back(asset);

		// Index asset by its name
		resourcesByName.insert(pair(asset->Name, asset));

		// Index the asset by its id
		resourcesById.insert(pair(asset->Uid, asset));

		LogMessage(
			"Discovered " + string(asset->Type) + string(" asset#: ") + to_string(asset->Uid) + string(" ") + string(
				asset->Name));
		countResources++;
	}

	string ResourceManager::GetSubscriberName() { return "resource manager"; }
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const string& name) { return resourcesByName[name]; }
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const int uuid) { return resourcesById[uuid]; }

	ResourceManager* ResourceManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new ResourceManager();
		}

		return Instance;
	}

	ResourceManager* ResourceManager::Instance = nullptr;
}
