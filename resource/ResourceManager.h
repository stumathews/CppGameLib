#pragma once
#include <string>
#include <map>
#include <memory>
#include "asset/asset.h"
#include "audio/AudioManager.h"
#include "events/EventManager.h"
#include "events/EventSubscriber.h"
#include "font/FontManager.h"
#include "graphic/SDLGraphicsManager.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>

namespace gamelib
{
	/***
	 * co-ordinates the resources in the game - such as holding definitions of all the resources/assets in the game
	 */
	class ResourceManager : public EventSubscriber
	{		
	    public:
		ResourceManager(SettingsManager& config, SDLGraphicsManager& graphics_admin, FontManager& font_admin, AudioManager&, Logger& the_logger);
			
		std::shared_ptr<asset> get(const std::string& name);
		std::shared_ptr<asset> get(int uuid);
		int get_resource_count() const { return resource_count; }
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> the_event) override;
		void unload();
		
		bool initialize(EventManager& event_admin);
	    std::string get_subscriber_name() override;

		// index the resoures file
		void IndexResources(std::string resources_file_path = "game/resources.xml");
		std::shared_ptr<gamelib::asset>& CreateAssetFromElement(const char* type, std::shared_ptr<gamelib::asset>& the_asset, tinyxml2::XMLElement* const& element);
		int get_resource_unloaded_count() const { return unloaded_resources_count; }
	    int get_resource_loaded_count() const { return loaded_resources_count; }
	private:
		SettingsManager& config;
		SDLGraphicsManager& graphics_admin;
		FontManager& font_admin;
		AudioManager& audio_admin;
		Logger& the_logger;
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

