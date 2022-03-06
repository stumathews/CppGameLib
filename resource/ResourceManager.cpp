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

	
	ResourceManager::ResourceManager(SettingsManager& config,   // NOLINT(modernize-pass-by-value)
		SDLGraphicsManager& graphics_admin, 
		FontManager& font_admin,
		AudioManager& audio_admin, Logger& the_logger)
	: config(config), graphics_admin(graphics_admin), font_admin(font_admin), audio_admin(audio_admin), the_logger(the_logger)
	{
		
	}

	bool ResourceManager::initialize(EventManager& eventAdmin)
	{
		return run_and_log("ResourceManager::initialize()", config.get_bool("global", "verbose"), [&]()
		{			
			eventAdmin.subscribe_to_event(event_type::LevelChangedEventType, this ); // we will load the resources for the level that has been loaded
			return true;
		}, config, true, true);
	}

	vector<shared_ptr<event>> ResourceManager::handle_event(const shared_ptr<event> the_event)
	{
		if(the_event->type == event_type::LevelChangedEventType)
		{
			run_and_log("Detected level change. Loading level assets...", config.get_bool("global", "verbose"), [&]()
			{
				load_level_assets(dynamic_pointer_cast<scene_changed_event>(the_event)->scene_id);
				return true;
			}, config, true, true);
		}

		return vector<shared_ptr<event>>();
	}

	/**
	 Load all the resources required by the scene and unload out all those that are not in the scene.
	 */
	void ResourceManager::load_level_assets(const int level)
	{	
		for(const auto& level_resources : resources_by_scene) // we need access to all resources to swap in/out resources
		{
			for(const auto& asset : level_resources.second)
			{
				const auto always_load_resource = asset->scene == 0;
				if((asset->scene == level || always_load_resource) && !asset->is_loaded)
				{				
					asset->load();
						
					the_logger.log_message(string("scene: " + to_string(asset->scene) ) + string(asset->name) + " asset loaded.");
						
					loaded_resources_count++;
					unloaded_resources_count--;
				} 
				else if(asset->is_loaded && asset->scene != level && !always_load_resource )
				{
					asset->unload();
						
					the_logger.log_message(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
					unloaded_resources_count++;
					loaded_resources_count--;
				}
			}		
		}
	}

	void ResourceManager::unload()
	{
		run_and_log("Unloading all resources...", config.get_bool("global", "verbose"), [&]()
		{
			for(const auto &item : resource_by_name)
			{
				auto &asset_name = item.first;
				auto &asset = item.second;
				
				asset->unload();
				
				log_message("Unloaded asset '" + asset_name + string("'."), the_logger);
			}		
			return true;
		}, config, true, true);	
	}
		
	// Index the Resources.xml file	 
	void ResourceManager::IndexResources(string resources_file_path)
	{	
		the_logger.log_message("ResourceManager: reading resources.xml.");
		
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
						shared_ptr<asset> theAsset = nullptr;
						const char* ptrAssetType;

						// Fetch type of the current asset
						element->QueryStringAttribute("type", &ptrAssetType);			

						// Create an asset for asset type						
						theAsset = CreateAssetFromElement(ptrAssetType, theAsset, element);						

						// Store the asset reference
						if(theAsset)
						{
							store_asset(theAsset);
						}
						else
						{
							the_logger.log_message(string("No asset manager defined for ") + ptrAssetType);
						}
					}
				}
			}
		}
		else
		{
			throw exception("Failed to load resources file");
		}

		log_message(to_string(resource_count) + string(" assets available in resource manager."), the_logger);
	}

	std::shared_ptr<gamelib::asset>& ResourceManager::CreateAssetFromElement(const char* type, std::shared_ptr<gamelib::asset>& the_asset, tinyxml2::XMLElement* const& element)
	{
		// Create a graphic asset
		if (strcmp(type, "graphic") == 0)
			the_asset = graphics_admin.create_asset(element, config);
		
		// Create fx/sound/audio asset
		if(strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
			the_asset = audio_admin.create_asset(element, *this);

		// Create font asset
		if(strcmp(type, "font") == 0)
			the_asset = font_admin.create_asset(element);

		return the_asset;
	}

	void ResourceManager::store_asset(const shared_ptr<asset>& theAsset)
	{
		// assets are explicitly associated with a scene that it will work in
		resources_by_scene[theAsset->scene].push_back(theAsset);

		// Index asset by its name
		resource_by_name.insert(pair<string, shared_ptr<asset>>(theAsset->name, theAsset));

		// Index the asset by its id
		resources_by_uuid.insert(pair<int, shared_ptr<asset>>(theAsset->uid, theAsset));

		log_message("Discovered " + string(theAsset->type) + string(" asset#: ") + to_string(theAsset->uid) + string(" ") + string(theAsset->name), the_logger);
		resource_count++;
	}

	string ResourceManager::get_subscriber_name()
	{
		return "resource manager";	
	}

	shared_ptr<asset> ResourceManager::get(const string& name)
	{
		return resource_by_name[name];		
	}

	shared_ptr<asset> ResourceManager::get(const int uuid)
	{
		return resources_by_uuid[uuid];
	}

}