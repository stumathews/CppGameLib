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

namespace gamelib
{	
	using namespace tinyxml2;
	using namespace std;
		
	ResourceManager::ResourceManager(SettingsManager& config, 
		SDLGraphicsManager& graphics_admin, 
		FontManager& font_admin, 	
		AudioManager& audio_admin, 
		Logger& the_logger)
		: config(config), 
		graphics_admin(graphics_admin),
		font_admin(font_admin), 
		audio_admin(audio_admin), 
		logger(the_logger)
	{ }

	/// <summary>
	/// Initialise the resrouce manager
	/// </summary>
	/// <param name="eventAdmin">Event admin</param>
	/// <returns>true if initialised, false otherwise</returns>
	bool ResourceManager::Initialize(EventManager& eventAdmin)
	{
		return LogThis("ResourceManager::initialize()", config.get_bool("global", "verbose"), [&]()
		{			
			eventAdmin.SubscribeToEvent(EventType::LevelChangedEventType, this ); // we will load the resources for the level that has been loaded
			return true;
		}, config, true, true);
	}

	// Handle events resource manager subscribes to
	vector<shared_ptr<Event>> ResourceManager::HandleEvent(const shared_ptr<Event> the_event)
	{
		if(the_event->type == EventType::LevelChangedEventType)
		{
			LogThis("Detected level change. Loading level assets...", config.get_bool("global", "verbose"), [&]()
			{
				LoadSceneAssets(dynamic_pointer_cast<SceneChangedEvent>(the_event)->scene_id);
				return true;
			}, config, true, true);
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
						
					logger.LogThis(string("scene: " + to_string(asset->scene) ) + string(asset->name) + " asset loaded.");
						
					loaded_resources_count++;
					unloaded_resources_count--;
				} 
				else if(asset->isLoadedInMemory && asset->scene != level && !always_load_resource )
				{
					asset->Unload();
						
					logger.LogThis(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
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
		LogThis("Unloading all resources...", config.get_bool("global", "verbose"), [&]()
		{
			for(const auto &item : resource_by_name)
			{
				auto &asset_name = item.first;
				auto &asset = item.second;
				
				asset->Unload();
				
				LogMessage("Unloaded asset '" + asset_name + string("'."), logger);
			}		
			return true;
		}, config, true, true);	
	}
		
	/// <summary>
	/// Index the Resources.xml file	 
	/// </summary>
	void ResourceManager::IndexResources(string resources_file_path)
	{	
		logger.LogThis("ResourceManager: reading resources.xml.");
		
		XMLDocument doc;
		
		doc.LoadFile( resources_file_path.c_str() ); // Load the list of resources
		
		if(doc.ErrorID() == 0)
		{		
			XMLNode* pResourceTree = doc.FirstChildElement("Assets");
				
			if(pResourceTree)
			{
				for(auto child = pResourceTree->FirstChild(); child; child = child->NextSibling())
				{
					auto* const element = child->ToElement();
					if(element)
					{
						shared_ptr<Asset> theAsset = nullptr;
						const char* ptrAssetType;

						// Fetch type of the current asset
						element->QueryStringAttribute("type", &ptrAssetType);			

						// Create an asset for asset type						
						theAsset = CreateAssetInfoFromElement(ptrAssetType, theAsset, element);						

						// Store the asset reference
						if(theAsset)
						{
							StoreAssetInfo(theAsset);
						}
						else
						{
							logger.LogThis(string("No asset manager defined for ") + ptrAssetType);
						}
					}
				}
			}
		}
		else
		{
			throw exception("Failed to load resources file");
		}

		LogMessage(to_string(resource_count) + string(" assets available in resource manager."), logger);
	}

	/// <summary>
	/// Creates Asset Info from an entry in the resource file
	/// </summary>
	/// <param name="type">Type of asset identified in the resource file</param>
	/// <param name="the_asset">The asset info</param>
	/// <param name="element">the current element containing the asset data</param>
	/// <returns></returns>
	std::shared_ptr<gamelib::Asset>& ResourceManager::CreateAssetInfoFromElement(const char* type, std::shared_ptr<gamelib::Asset>& assetInfo, tinyxml2::XMLElement* const& element)
	{
		// Create a graphic asset
		if (strcmp(type, "graphic") == 0)
			assetInfo = graphics_admin.create_asset(element, config);
		
		// Create fx/sound/audio asset
		if(strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
			assetInfo = audio_admin.create_asset(element, *this);

		// Create font asset
		if(strcmp(type, "font") == 0)
			assetInfo = font_admin.create_asset(element);

		return assetInfo;
	}

	/// <summary>
	/// Indexes the asset info
	/// </summary>
	/// <param name="theAsset">the asset info to store</param>
	void ResourceManager::StoreAssetInfo(const shared_ptr<Asset>& theAsset)
	{
		// assets are explicitly associated with a scene that it will work in
		resources_by_scene[theAsset->scene].push_back(theAsset);

		// Index asset by its name
		resource_by_name.insert(pair<string, shared_ptr<Asset>>(theAsset->name, theAsset));

		// Index the asset by its id
		resources_by_uuid.insert(pair<int, shared_ptr<Asset>>(theAsset->uid, theAsset));

		LogMessage("Discovered " + string(theAsset->type) + string(" asset#: ") + to_string(theAsset->uid) + string(" ") + string(theAsset->name), logger);
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