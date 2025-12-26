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
#include "file/ScriptManager.h"

namespace gamelib
{
	using namespace tinyxml2;
	using namespace std;

	ResourceManager::ResourceManager(): debug(false) {  }
	ResourceManager::~ResourceManager() { instance = nullptr; }

	/// <summary>
	/// Initialize the resource manager
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
		for (const auto& kv : resourcesByScene) // we need access to all resources to swap in/out resources
		{
			const auto& val = kv.second;
			for (const auto& asset : val)
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
		std::stringstream message;

		// Start again from fresh - no resources
		Reset();

		message << "ResourceManager: reading file: " << resourcesFilePath.c_str() << "\n";
		Logger::Get()->LogThis(message.str());
		message.clear();

		// Index resources from resource file (no loading occurs)
		XMLDocument xmlDocument;

		// Read the resources file
		xmlDocument.LoadFile(resourcesFilePath.c_str());

		// If not errors, parse the XML file
		if (xmlDocument.ErrorID() == 0)
		{

			message << "Loading Assets...\n";
			Logger::Get()->LogThis(message.str());

			// Get the Assets XML node
			auto* pAssetsNode = xmlDocument.FirstChildElement("Assets");

			if (pAssetsNode)
			{
				// Get all the assets in the XML file
				for (auto child = pAssetsNode->FirstChild(); child; child = child->NextSibling())
				{
					auto* const assetElement = child->ToElement();

					if (assetElement)
					{
						// Prepare to create the asset from the element definition
						shared_ptr<Asset> theAsset = nullptr;
						const char* ptrAssetType {};

						// Determine the asset type
						assetElement->QueryStringAttribute("type", &ptrAssetType);

						// Create the asset based on the asset type
						theAsset = CreateAssetFromElement(ptrAssetType, theAsset, assetElement);

						message << "Found asset Name="
								<< theAsset->Name
								<< " Type=" << theAsset->Type
								<< " SceneId=" << theAsset->SceneId
								<< " Uid=" << theAsset->Uid
								<< " FilePath="<< theAsset->FilePath << "\n";
						Logger::Get()->LogThis(message.str());

						if (theAsset != nullptr)
						{
							// Store the asset reference, but don't load it
							StoreAsset(theAsset);

							// Keep track of how many assets we have
							countResources++;

							Logger::Get()->LogThis(std::string("Asset: ") + theAsset->Name + " indexed by resource manager");
						}
						else
						{
							message.clear();
							message << "No asset manager defined for " << ptrAssetType << "\n";
							Logger::Get()->LogThis(message.str());
							THROW(static_cast<int>(ResourceManager::ErrorNumbers::NoAssetManagerForType), message.str(),
							      this->GetSubscriberName());
						}
					}
				}
			}
			else
			{
				THROW(12, "No Assets found in resource file", GetSubscriberName());
			}
		}
		else
		{
			message.clear();
			message << "Failed to load resources file '"
			<< resourcesFilePath.c_str()
			<< "'. ErrorId=" << xmlDocument.ErrorID()
			<< " ErrorName=" << xmlDocument.ErrorName()
			<< " Description=" << xmlDocument.ErrorStr() << "\n";
			THROW(static_cast<int>(ResourceManager::ErrorNumbers::FailedToLoadResourceFile),
			      message.str(), this->GetSubscriberName());
		}

		LogMessage(to_string(countResources) + string(" assets available in resource manager."));
	}

	/// <summary>
	/// Creates Asset Info from an entry in the resource file
	/// </summary>
	/// <param name="type">Type of asset identified in the resource file</param>
	/// <param name="asset"></param>
	/// <param name="assetElement">the current element containing the asset data</param>
	/// <returns></returns>
	std::shared_ptr<Asset>& ResourceManager::CreateAssetFromElement(const char* type, std::shared_ptr<Asset>& asset,
	                                                                XMLElement* const& assetElement)
	{
		// Create types of assets based on their definition

		// Create a graphic asset
		if (strcmp(type, "graphic") == 0)
		{
			// Read graphic asset details
			asset = GraphicAssetFactory::Get()->Parse(assetElement);
		}

		// Create fx/sound/audio asset
		else if (strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
		{
			// Read audio asset details
			asset = AudioManager::Get()->CreateAsset(assetElement, *this);
		}

		// Create font asset
		else if (strcmp(type, "font") == 0)
		{
			// Read font asset details
			asset = FontManager::Get()->CreateAsset(assetElement);
		}
		else if (strcmp(type, "script") == 0)
		{
			asset = ScriptManager::Get()->CreateAsset(assetElement);
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
	}

	string ResourceManager::GetSubscriberName() { return "resource manager"; }

	void ResourceManager::Reset()
	{
		Unload();
		resourcesById.clear();
		resourcesByName.clear();
		resourcesByScene.clear();
		countLoadedResources = 0;
		countResources = 0;
		countUnloadedResources = 0;
	}

	shared_ptr<Asset> ResourceManager::GetAssetInfo(const string& name) { return resourcesByName[name]; }
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const int uuid) { return resourcesById[uuid]; }

	ResourceManager* ResourceManager::Get()
	{
		if (instance == nullptr)
		{
			instance = new ResourceManager();
		}

		return instance;
	}

	ResourceManager* ResourceManager::instance = nullptr;
}
