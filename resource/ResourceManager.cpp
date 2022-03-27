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
#include <exceptions/base_exception.h>
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
		return LogThis("ResourceManager::initialize()", SettingsManager::Get()->get_bool("global", "verbose"), [&]()
		{			
			EventManager::Get()->SubscribeToEvent(EventType::LevelChangedEventType, this ); // we will load the resources for the level that has been loaded
			return true;
		}, true, true);
	}

	// Handle events resource manager subscribes to
	vector<shared_ptr<Event>> ResourceManager::HandleEvent(const shared_ptr<Event> the_event)
	{
		if(the_event->type == EventType::LevelChangedEventType)
		{
			LogThis("Detected level change. Loading level assets...", SettingsManager::Get()->get_bool("global", "verbose"), [&]()
			{
				LoadSceneAssets(dynamic_pointer_cast<SceneChangedEvent>(the_event)->scene_id);
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
		for(const auto& level_resources : resources_by_scene) // we need access to all resources to swap in/out resources
		{
			for(const auto& asset : level_resources.second)
			{
				const auto always_load_resource = asset->scene == 0;
				if((asset->scene == level || always_load_resource) && !asset->isLoadedInMemory)
				{				
					asset->Load();
						
					Logger::Get()->LogThis(string("scene: " + to_string(asset->scene) ) + string(asset->name) + " asset loaded.");
						
					loaded_resources_count++;
					unloaded_resources_count--;
				} 
				else if(asset->isLoadedInMemory && asset->scene != level && !always_load_resource )
				{
					asset->Unload();
						
					Logger::Get()->LogThis(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
					unloaded_resources_count++;
					loaded_resources_count--;
				}
			}		
		}
	}

	/// <summary>
	/// Ask each asset to unload itself
	/// </summary>
	void ResourceManager::Unload()
	{
		LogThis("Unloading all resources...", SettingsManager::Get()->get_bool("global", "verbose"), [&]()
		{
			for(const auto &item : resource_by_name)
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
	void ResourceManager::IndexResources(string resources_file_path)
	{	
		Logger::Get()->LogThis("ResourceManager: reading resources.xml.");
		
		XMLDocument doc;
		
		doc.LoadFile( resources_file_path.c_str() ); // Load the list of resources
		
		if(doc.ErrorID() == 0)
		{		
			XMLNode* pAssetsNode = doc.FirstChildElement("Assets");
				
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
		}
		else
		{
			THROW((int) ResourceManager::ErrorNumbers::FailedToLoadResourceFile, "Failed to load resources file", this->GetSubscriberName());
		}

		LogMessage(to_string(resource_count) + string(" assets available in resource manager."));
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
	void ResourceManager::StoreAsset(const shared_ptr<Asset>& theAsset)
	{
		// assets are explicitly associated with a scene that it will work in
		resources_by_scene[theAsset->scene].push_back(theAsset);

		// Index asset by its name
		resource_by_name.insert(pair<string, shared_ptr<Asset>>(theAsset->name, theAsset));

		// Index the asset by its id
		resources_by_uuid.insert(pair<int, shared_ptr<Asset>>(theAsset->uid, theAsset));

		LogMessage("Discovered " + string(theAsset->type) + string(" asset#: ") + to_string(theAsset->uid) + string(" ") + string(theAsset->name));
		resource_count++;
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
		return resource_by_name[name];		
	}

	/// <summary>
	/// Get Asset info by Id
	/// </summary>
	/// <param name="uuid">Id of the asset as found in asset file</param>
	/// <returns>Asset info</returns>
	shared_ptr<Asset> ResourceManager::GetAssetInfo(const int uuid)
	{
		return resources_by_uuid[uuid];
	}
}