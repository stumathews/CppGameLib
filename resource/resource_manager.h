#pragma once
#include <string>
#include <map>
#include <memory>
#include "asset/asset.h"
#include "audio/AudioManager.h"
#include "events/event_manager.h"
#include "events/EventSubscriber.h"
#include "font/font_manager.h"
#include "graphic/sdl_graphics_manager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	/***
	 * co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
	 */
	class resource_manager final : public EventSubscriber, public inheritable_enable_shared_from_this<resource_manager>
	{		
	    public:
		resource_manager(std::shared_ptr<settings_manager> config, std::shared_ptr<sdl_graphics_manager> graphics_admin, std::shared_ptr<font_manager> font_admin, std::shared_ptr<audio_manager>, std::shared_ptr<logger> the_logger = std::make_shared<logger>());
			
		std::shared_ptr<asset> get(const std::string& name);
		std::shared_ptr<asset> get(int uuid);
		int get_resource_count() const { return resource_count; }
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
		void unload();
		
		bool initialize(std::shared_ptr<event_manager> event_admin);
	    std::string get_subscriber_name() override;
		void read_resources(std::string resources_file_path = "game/resources.xml");
		int get_resource_unloaded_count() const { return unloaded_resources_count; }
	    int get_resource_loaded_count() const { return loaded_resources_count; }
	private:
		std::shared_ptr<settings_manager> config;
		std::shared_ptr<sdl_graphics_manager> graphics_admin;
		std::shared_ptr<font_manager> font_admin;
		std::shared_ptr<audio_manager> audio_admin;
		std::shared_ptr<logger> the_logger;
		void load_level_assets(int level);
	    void store_asset(const std::shared_ptr<asset>& the_asset);
		std::map<int, std::vector<std::shared_ptr<asset>>> resources_by_scene;   
		std::map<std::string, std::shared_ptr<asset>> resource_by_name;   
		std::map<int, std::shared_ptr<asset>> resources_by_uuid;

		int resource_count = 0;
		int loaded_resources_count = 0;
		int unloaded_resources_count = 0;
	};
}

