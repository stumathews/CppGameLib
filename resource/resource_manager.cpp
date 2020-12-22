#include "resource_manager.h"
#include "tinyxml2.h"
#include <iostream>
#include <map>
#include <memory>
//#include "game/exceptions/game_exception.h"
#include "audio/AudioManager.h"
#include "common/Common.h"
#include "common/global_config.h"
#include "events/event_manager.h"
#include "events/SceneChangedEvent.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"

namespace gamelib
{	
	using namespace tinyxml2;
	using namespace std;

	
	resource_manager::resource_manager(shared_ptr<global_config> c, shared_ptr<sdl_graphics_manager> g, shared_ptr<font_manager> f, shared_ptr<audio_manager> a)
	: config(c), graphics_admin(g), font_admin(f), audio_admin(a)
	{
		
	}

	bool resource_manager::initialize(shared_ptr<event_manager> event_admin)
	{
		return run_and_log("resource_manager::initialize()", config->verbose, [&]()
		{			
			event_admin->subscribe_to_event(event_type::LevelChangedEventType, this); // we will load the resources for the level that has been loaded
			return true;
		});
	}

	vector<shared_ptr<event>> resource_manager::handle_event(const shared_ptr<event> the_event)
	{
		if(the_event->type == event_type::LevelChangedEventType)
		{
			run_and_log("Detected level change. Loading level assets...", config->verbose, [&]()
			{
				load_level_assets(dynamic_pointer_cast<scene_changed_event>(the_event)->scene_id);
				return true;
			});
		}

		return vector<shared_ptr<event>>();
	}

	/**
	 Load all the resources required by the scene and unload out all those that are not in the scene.
	 */
	void resource_manager::load_level_assets(const int level)
	{	
		for(const auto& level_resources : resources_by_scene) // we need access to all resources to swap in/out resources
		{
			for(const auto& asset : level_resources.second)
			{
				const auto always_load_resource = asset->scene == 0;
				if((asset->scene == level || always_load_resource) && !asset->is_loaded)
				{				
					asset->load();
						
					logger::log_message(string("scene: " + to_string(asset->scene) ) + string(asset->name) + " asset loaded.");
						
					loaded_resources_count++;
					unloaded_resources_count--;
				} 
				else if(asset->is_loaded && asset->scene != level && !always_load_resource )
				{
					asset->unload();
						
					logger::log_message(string("scene: " + to_string(asset->scene))  + string(asset->name) + " asset unloaded.");
					unloaded_resources_count++;
					loaded_resources_count--;
				}
			}		
		}
	}

	void resource_manager::unload()
	{
		run_and_log("Unloading all resources...", config->verbose, [&]()
		{
			for(const auto &item : resource_by_name)
			{
				auto &asset_name = item.first;
				auto &asset = item.second;
				
				asset->unload();
				
				log_message("Unloaded asset '" + asset_name + string("'."));
			}		
			return true;
		});	
	}



	/**
	 Index Resources.xml file
	 */
	void resource_manager::read_resources(string resources_file_path)
	{	
		logger::log_message("resource_manager: reading resources.xml.");
		
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
						shared_ptr<asset> the_asset = nullptr;
						const char* type;

						element->QueryStringAttribute("type", &type);			

						if(strcmp(type, "graphic") == 0)
							the_asset = graphics_admin->create_asset(element, config);

						if(strcmp(type, "fx") == 0 || strcmp(type, "music") == 0)
							the_asset = audio_admin->create_asset(element, shared_from_this());

						if(strcmp(type, "font") == 0)
							the_asset = font_admin->create_asset(element);

						if(the_asset)
							store_asset(the_asset);
						else
							logger::log_message(string("No asset manager defined for ") + type);
					}
				}
			}
		}
		else
		{
			throw exception("Failed to load resources file");
		}

		log_message(to_string(resource_count) + string(" assets available in resource manager."));
	}

	void resource_manager::store_asset(const shared_ptr<asset>& the_asset)
	{
		// assets are explicitly associated with a scene that it will work in
		resources_by_scene[the_asset->scene].push_back(the_asset);

		// Index asset by its name
		resource_by_name.insert(pair<string, shared_ptr<asset>>(the_asset->name, the_asset));

		// Index the asset by its id
		resources_by_uuid.insert(pair<int, shared_ptr<asset>>(the_asset->uid, the_asset));

		log_message("Discovered " + string(the_asset->type) + string(" asset#: ") + to_string(the_asset->uid) + string(" ") + string(the_asset->name));
		resource_count++;
	}

	string resource_manager::get_subscriber_name()
	{
		return "resource manager";	
	}

	shared_ptr<asset> resource_manager::get(const string& name)
	{
		return resource_by_name[name];		
	}

	shared_ptr<asset> resource_manager::get(const int uuid)
	{
		return resources_by_uuid[uuid];
	}

}