#include "SceneManager.h"
#include <list>
#include "tinyxml2.h"
#include <memory>
#include <list>

#include "events/GameObjectEvent.h"
#include "common/Common.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/EventManager.h"
#include "events/SceneChangedEvent.h"
#include "events/scene_loaded_event.h"
#include "objects/game_object_factory.h"

namespace gamelib
{
	

	bool SceneManager::initialize()
	{
		is_initialized = run_and_log("SceneManager::initialize()", config.get_bool("global", "verbose"), [&]()
		{
			// I care about when the level changes
			event_admin.subscribe_to_event(event_type::LevelChangedEventType, this);

			// I care about when I'm asked to add game object to current scene
			event_admin.subscribe_to_event(event_type::AddGameObjectToCurrentScene, this);
			event_admin.subscribe_to_event(event_type::GenerateNewLevel, this);
			event_admin.subscribe_to_event(event_type::GameObject, this);

			return true;
		}, config, true, true);
		return is_initialized;
	}

	void SceneManager::start_scene(int scene_id)
	{
		// Emit event to switch to scene 1 on initialization of scene manager.
		// This triggers usually the ResourceManager that loads the resources in for the scene (see ResourceManager::process_events etc..)
		event_admin.raise_event(std::make_shared<scene_changed_event>(scene_id), this);
	}



	SceneManager::SceneManager(EventManager& ea, SettingsManager& c, ResourceManager& resource_admin, game_world_data& world, gamelib::Logger& Logger, std::string scene_folder)
	: event_admin(ea), config(c), resource_admin(resource_admin), scene_folder(scene_folder), world(world), Logger(Logger)
	{
	}

	vector<shared_ptr<event>> SceneManager::handle_event(const std::shared_ptr<event> the_event)
	{
		switch(the_event->type)
		{
			case event_type::PositionChangeEventType: break;
			case event_type::LevelChangedEventType: // load in new scene
			{
				load_new_scene(the_event, resource_admin);
			}
			break;
			case event_type::DoLogicUpdateEventType: break;	
			case event_type::AddGameObjectToCurrentScene:  // add new object to scene (last layer)
			{
				const auto the_game_object = std::dynamic_pointer_cast<add_game_object_to_current_scene_event>(the_event)->get_game_object();
				add_to_scene(the_game_object);
			}
			break;
			case event_type::PlayerMovedEventType: break;
			case event_type::scene_loaded: break;
			case event_type::GameObject:
				const auto game_object_event = dynamic_pointer_cast<GameObjectEvent>(the_event);
				if( game_object_event->context == GameObjectEventContext::Remove)
					remove_from_layers(game_object_event->game_object->id);				
				break;
		}
		
		return vector<shared_ptr<event>>();
	}

	void SceneManager::remove_from_layers(int game_object_id)
	{
		for_each(begin(layers), end(layers), [&](shared_ptr<layer> layer)
		{
			layer->game_objects.remove_if([=](weak_ptr<GameObject> game_object)
			{
				if(auto ptr = game_object.lock())
				{
					return ptr->id == game_object_id;
				}
			});
			
		});
	}

	void SceneManager::load_new_scene(const std::shared_ptr<event>& the_event, ResourceManager& resource_admin)
	{
		const auto scene =  std::dynamic_pointer_cast<scene_changed_event>(the_event)->scene_id;

		auto raise_scene_loaded_event = [this](int scene_id, const string& scene_name)
		{
			log_message("Scene "+ to_string(scene_id) +" : "+ scene_name +" loaded.", Logger);
			event_admin.raise_event(make_unique<scene_loaded_event>(scene_id), this);
		};

		string scene_name;
		
		switch(scene)
		{
		case 1:			
			scene_name = scene_folder + "scene1.xml";
			break;
		case 2:
			scene_name = scene_folder + "scene2.xml";
			break;
		case 3:
			scene_name = scene_folder +"scene3.xml";
			break;
		case 4:
			scene_name = scene_folder + "scene4.xml";
			break;
		default:
			scene_name = scene_folder + "scene1.xml";
		}

		if(!scene_name.empty())
		{
			try 
			{
				load_scene_file(scene_name, resource_admin);
				raise_scene_loaded_event(scene, scene_name);
			} catch(exception &e)
			{
				log_message(string("Cloud not load scene file: ") + string(e.what()), Logger);
				throw;
			}
		}
	}

	void SceneManager::add_to_scene(const std::shared_ptr<GameObject>& game_object)
	{
		// add to last layer of the scene
		layers.back()->game_objects.push_back(game_object);
	}

	shared_ptr<layer> SceneManager::add_layer(const std::string& name)
	{
		auto the_layer = find_layer(name);
		if(!the_layer)
		{
			the_layer = std::make_shared<layer>();
			the_layer->name = name;
			layers.push_back(the_layer);
		}
		return the_layer;
	}

	shared_ptr<layer> SceneManager::find_layer(const std::string& name)
	{
		for(const auto& layer : layers)
			if(layer->name == name)
				return layer;		
		
		return nullptr;
	}

	void SceneManager::remove_layer(const std::string& name)
	{
		for(const auto& layer : layers)
			if(layer->name == name)
				layers.remove(layer);
	}

	bool compare_layer_order(const shared_ptr<layer> rhs, const shared_ptr<layer> lhs)
	{
		return lhs->zorder < rhs->zorder;
	}

	void SceneManager::sort_layers()
	{
		//std::sort(begin(layers), end(layers)); // can we use this?
		layers.sort(compare_layer_order);
	}

	void SceneManager::update()
	{
		// Scene manager does not need updating
	}

	std::list<shared_ptr<layer>> SceneManager::get_scene_layers() const
	{
		return layers;
	}


	bool SceneManager::load_scene_file(const std::string& filename, ResourceManager& resource_admin)
	{
		if(current_scene_name == filename)
		{
			log_message(string("Scene already loaded. Skipping."), Logger);
			return true;
		}
		log_message("Loading scene: " + string(filename), Logger);
		
		/* A Scene is composed of a) resources at b) various positions c) visibility
		
		<scene id="2">
		  <layer name="layer0" posx="0" posy="0" visible="true">
			<objects>
			  <object posx="100" posy="40" resourceId="7" visible="true" colourKey="true" r="0" g="0" b="0"></object>
			  <object posx="500" posy="40" resourceId="8" visible="true" colourKey="true" r="0" g="0" b="0"></object>
			</objects>
		  </layer>
		</scene>
		
		Read in the scene details and store them in the scene manager
		*/

		tinyxml2::XMLDocument doc;		
		doc.LoadFile(filename.c_str());
		if(doc.ErrorID() == 0) 	
		{
			tinyxml2::XMLNode* scene = doc.FirstChildElement("scene");
			auto scene_id = scene->ToElement()->Attribute("id");
			
			if(scene) // <scene id="2">
			{
				for(auto layer_node = scene->FirstChild(); layer_node; layer_node = layer_node->NextSibling()) //  <layer ...>
				{				
					auto layer_element = layer_node->ToElement();
					if(layer_element) 
					{
						// build up a layer object from scene file
						auto the_layer = std::make_shared<layer>();
						the_layer->zorder = layers.size();					

						for(auto layer_attributes = layer_element->FirstAttribute(); layer_attributes; layer_attributes = layer_attributes->Next()) // // <layer name="layer0" posx="0" posy="0" visible="true"
						{						
							// populate the new layer object:
							
							const string key(layer_attributes->Name());
							const string value(layer_attributes->Value());						

							if(key == "name")
							{
								the_layer->name = value;
								continue;
							}

							if(key == "posx")
							{
								the_layer->x = static_cast<int>(std::atoi(value.c_str()));
								continue;
							}

							if(key == "posy") 
							{
								the_layer->y = static_cast<int>(std::atoi(value.c_str()));
								continue;
							}

							if(key == "visible")
								the_layer->visible = (value == "true") ? true : false;
						}

						// Process contents of the layer 
						for(auto layer_item = layer_node->FirstChild(); layer_item; layer_item = layer_item->NextSibling()) // <object ...
						{
							if(std::string(layer_item->Value()) == "objects") 
							{
								for(auto object_node = layer_item->FirstChild(); object_node; object_node = object_node->NextSibling())
								{	
									auto object = object_node->ToElement();
									if(object == nullptr)
										continue;

									//
									auto game_object = game_object_factory::get_instance().build_game_object(object, resource_admin, config, event_admin);
									world.game_objects.push_back(game_object);

									// Keep track of the game object in this layer
									the_layer->game_objects.push_back(game_object);																
								}
							}
						}

						// populate the number of layers in this scene
						layers.push_back(the_layer);
					}
				}
				
				sort_layers(); // We want to draw from zOrder 0 -> onwards (in order)
				current_scene_name = filename;
				return true;
			} // finished processing scene, layers populated

		}
		return false;
	}

	string SceneManager::get_subscriber_name()
	{
		return "SceneManager";
	}

}
