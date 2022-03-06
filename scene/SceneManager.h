#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include <list>
#include "Layer.h"
#include "events/EventSubscriber.h"
#include "resource/ResourceManager.h"
#include "objects/game_world_data.h"
#include <objects/MultipleInheritableEnableSharedFromThis.h>
#include <common/Logger.h>

using namespace std;

namespace gamelib
{
	// Represents the current scene
	class SceneManager final : public EventSubscriber, public inheritable_enable_shared_from_this<IEventSubscriber>
	{
		EventManager& event_admin;
		SettingsManager& config;
		ResourceManager& resource_admin;
		game_world_data& world;
		Logger& Logger;
	public:
		SceneManager(EventManager& ea, SettingsManager& c, ResourceManager& resource_admin, game_world_data& world, gamelib::Logger& Logger, std::string scene_folder = "game/");	
		SceneManager(const SceneManager &) = delete;
		SceneManager(SceneManager &&) = delete;
	    SceneManager& operator=(SceneManager const&)  = delete;
		SceneManager& operator=(SceneManager &&) = delete;
		virtual ~SceneManager() = default;

		bool initialize();	
		void start_scene(int scene_id);
		list<shared_ptr<layer>> get_scene_layers() const;
	private:
		void add_to_scene(const shared_ptr<GameObject>& game_object);	
		void load_new_scene(const shared_ptr<event> &the_event, ResourceManager& resource_admin);
		bool load_scene_file(const string &filename, ResourceManager& resource_admin);		
		void remove_layer(const string &name);
		void sort_layers();
		shared_ptr<layer> add_layer(const string &name);
		shared_ptr<layer> find_layer(const string &name);

		// Event management
		static void update();
		vector<shared_ptr<event>> handle_event(shared_ptr<event> the_event) override;
		void remove_from_layers(int game_object_id);
		string get_subscriber_name() override;
			
		list<shared_ptr<layer>> layers;
		string current_scene_name = {};
		bool is_initialized = false;
		std::string scene_folder;
	};
}
#endif