#include "ResourceManager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include <memory>
//#include "game/exceptions/game_exception.h"
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "events/EventManager.h"
#include "events/SceneChangedEvent.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include <exceptions/EngineException.h>
#include "graphic/GraphicAssetFactory.h"

namespace gamelib
{	
	using namespace tinyxml2;
	using namespace std;
		
	ResourceManager::ResourceManager()
	{ }

	ResourceManager* ResourceManager::Get()
	{
		if (Instance == nullptr)
		{
			Instance = new ResourceManager();
		}
		return Instance;
	}

	ResourceManager::~ResourceManager()
	{
		Instance = nullptr;
	}

	ResourceManager* ResourceManager::Instance = nullptr;

	/// <summary>
	/// Initialise the resrouce manager
	/// </summary>
	/// <param name="eventAdmin">Event admin</param>
	/// <returns>true if initialised, false otherwise</returns>
	bool ResourceManager::Initialize()
	{
		IndexResourceFile();
		debug = SettingsManager::Get()->GetBool("global", "verbose");
		return LogThis("ResourceManager::initialize()", debug, [&]()
		{			
			EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this ); // we will load the resources for the level that has been loaded
			return true;
		}, true, true);
	}

	/// <summary>
	/// Handle events resource manager subscribes to
	/// </summary>
	/// <param name="event"></param>
	/// <returns></returns>
	vector<shared_ptr<Event>> ResourceManager::HandleEvent(const shared_ptr<Event> event)
	{
		if(event->type == EventType::LevelChangedEventType)
		{
			LogThis("Detected level change. Loading level assets...", debug, [&]()
			{
				LoadSceneAssets(dynamic_pointer_cast<SceneChangedEvent>(event)->scene_id);
				return true;
			}, true, true);
		}

		return vector<shared_ptr<Event>>();
	}

	/// <summary>
	/// Load all the resources required by the scene and unload out all those that are not in the scene.
	/// </summary>
	/// <remarks>Other scene assets are unloaded</remarks>
	/// <param name="level">Scene/Level assets to load.</param>
	void ResourceManager::LoadSceneAssets(const int level)
	{	
		for(const auto& level_resources : resourcesByScene) // we need access to all resources to swap in/out resources
		{
			for(const auto& asset : level_resources.second)
			{
				const auto always_load_resource = asset->scene == 0;
				if((asset->scene == level || always_load_resource) && !asset->isLoadedInMemory)
				{				
					asset->Load();
					
					Logger::Get()->LogThis(string("scene " + to_string(asset->scene) ) +string(": ") + string(asset->name) + " asset loaded.");
						
					countLoadedResources++;
					countUnloadedResources--;
				} 
				else if(asset->isLoadedInMemory && asset->scene != level && !always_load_resource )
				{
					asset->Unload();
						
					Logger::Get()->LogThis(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
					countUnloadedResources++;
					countLoadedResources--;
				}
			}		
		}
	}

	/// <summary>
	/// Ask each asset to unload itself
	/// </summary>
	void ResourceManager::Unload()
	{
		LogThis("Unloading all resources...", debug, [&]()
		{
			for(const auto &item : resourcesByName)
			{
				auto &asset_name = item.first;
				auto &asset = item.second;
				
				asset->Unload();
				
				LogMessage("Unloaded asset '" + asset_name + string("'."));
			}		
			return true;
		}, true, true);	
	}
		
	/// <summary>
	/// Index the Resources.xml file	 
	/// </summary>
	void ResourceManager::IndexResourceFile(string resourceFilePath)
	{	
		Logger::Get()->LogThis("ResourceManager: reading resources.xml.");
		
		XMLDocument xmlDocument;
		
		// Load resource file
		xmlDocument.LoadFile(resourceFilePath.c_str()); 
		
		// Parse resource file
		if(xmlDocument.ErrorID() == 0)
		{		
			auto* pAssetsNode = xmlDocument.FirstChildElement("Assets");
				
			if(pAssetsNode)
			{
				for(auto child = pAssetsNode->FirstChild(); child; child = child->NextSibling())
				{
					auto* const element = child->ToElement();
					if(element)
					{
						shared_ptr<Asset> theAsset = nullptr;
						const char* ptrAssetType;

						// Fetch type of the current asset
						element->QueryStringAttribute("type", &ptrAssetType);			

						// Create an asset for asset type						
						theAsset = CreateAssetFromElement(ptrAssetType, theAsset, element);						

						// Store the asset reference, but dont load it
						if(theAsset != nullptr)
						{
							StoreAsset(theAsset);
						}
						else
						{
							auto message = string("No asset manager defined for ") + ptrAssetType;
							Logger::Get()->LogThis(message);
							THROW((int)ResourceManager::ErrorNumbers::NoAssetManagerForType, message, this->GetSubscriberName());
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
			THROW((int) ResourceManager::ErrorNumbers::FailedToLoadResourceFile, "Failed to load resources file", this->GetSubscriberName());
		}

		LogMessage(to_string(countResources) + string(" assets available in resource manager."));
	}

	/// <summary>
	/// Creates Asset Info from an entry in the resource file
	/// </summary>
	/// <param name="type">Type of asset identified in the resource file</param>
	/// <param name="the_asset">The asset info</param>
	/// <param name="element">the current element containing the asset data</param>
	/// <returns></returns>
	std::shared_ptr<gamelib::Asset>& ResourceManager::CreateAssetFromElement(const char* type, std::shared_ptr<gamelib::Asset>& asset, tinyxml2::XMLElement* const& element)
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
			auto message = string("Unknown resource type:") + type;
			THROW((int)ResourceManager::ErrorNumbers::UnknownResourceType, message, GetSubscriberName());
		}

		return asset;
	}

	/// <summary>
	/// Indexes the asset info
	/// </summary>
	/// <param name="theAsset">the asset info to store</param>
	void ResourceManager::StoreAsset(const shared_ptr<Asset>& asset)
	{
		// assets are explicitly associated with a scene that it will work in
		resourcesByScene[asset->scene].push_back(asset);

		// Index asset by its name
		resourcesByName.insert(pair<string, shared_ptr<Asset>>(asset->name, asset));

		// Index the asset by its id
		resourcesById.insert(pair<int, shared_ptr<Asset>>(asset->uid, asset));

		LogMessage("Discovered " + string(asset->type) + string(" asset#: ") + to_string(asset->uid) + string(" ") + string(asset->name));
		countResources++;
	}

	/// <summary>
	/// Provide subscriber name to event system
	/// </summary>
	/// <returns>Name of resource manager </returns>
	string ResourceManager::GetSubscriberName()
	{
		return "resource manager";	
	}

	/// <summary>
	/// Get Asset Info by Name
	/// </summary>
	/// <param name="name">Name of the asset</param>
	/// <returns>Assert info</returns>
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const string& name)
	{
		return resourcesByName[name];		
	}

	/// <summary>
	/// Get Asset info by Id
	/// </summary>
	/// <param name="uuid">Id of the asset as found in asset file</param>
	/// <returns>Asset info</returns>
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const int uuid)
	{
		return resourcesById[uuid];
	}
}