#include "scene_manager.h"
#include <list>
#include "tinyxml2.h"
#include <memory>
#include <list>
#include "common/Common.h"
#include "events/AddGameObjectToCurrentSceneEvent.h"
#include "events/event_manager.h"
#include "events/SceneChangedEvent.h"
#include "events/scene_loaded_event.h"
#include "objects/game_object_factory.h"

namespace gamelib
{
	

	bool scene_manager::initialize()
	{
		is_initialized = run_and_log("scene_manager::initialize()", config->get_bool("global", "verbose"), [&]()
		{
			// I care about when the level changes
			event_admin->subscribe_to_event(event_type::LevelChangedEventType, this);

			// I care about when I'm asked to add game object to current scene
			event_admin->subscribe_to_event(event_type::AddGameObjectToCurrentScene, this);
			event_admin->subscribe_to_event(event_type::GenerateNewLevel, this);			

			return true;
		}, true, true, config);
		return is_initialized;
	}

	void scene_manager::start_scene(int scene_id)
	{
		// Emit event to switch to scene 1 on initialization of scene manager.
		// This triggers usually the resource_manager that loads the resources in for the scene (see resource_manager::process_events etc..)
		event_admin->raise_event(std::make_shared<scene_changed_event>(scene_id), this);
	}



	scene_manager::scene_manager(shared_ptr<event_manager> ea, shared_ptr<settings_manager> c, shared_ptr<resource_manager> resource_admin,  std::string scene_folder)
	: event_admin(ea), config(c), resource_admin(resource_admin), scene_folder(scene_folder)
	{
	}

	vector<shared_ptr<event>> scene_manager::handle_event(const std::shared_ptr<event> the_event)
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
				break;
		}
		
		return vector<shared_ptr<event>>();
	}

	void scene_manager::load_new_scene(const std::shared_ptr<event>& the_event, shared_ptr<resource_manager> resource_admin)
	{
		const auto scene =  std::dynamic_pointer_cast<scene_changed_event>(the_event)->scene_id;

		auto raise_scene_loaded_event = [this](int scene_id, const string& scene_name)
		{
			log_message("Scene "+ to_string(scene_id) +" : "+ scene_name +" loaded.");
			event_admin->raise_event(make_unique<scene_loaded_event>(scene_id), this);
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
				log_message(string("Cloud not load scene file: ") + string(e.what()));
				throw;
			}
		}
	}

	void scene_manager::add_to_scene(const std::shared_ptr<game_object>& game_object)
	{
		// add to last layer of the scene
		layers.back()->game_objects.push_back(game_object);
	}

	shared_ptr<layer> scene_manager::add_layer(const std::string& name)
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

	shared_ptr<layer> scene_manager::find_layer(const std::string& name)
	{
		for(const auto& layer : layers)
			if(layer->name == name)
				return layer;		
		
		return nullptr;
	}

	void scene_manager::remove_layer(const std::string& name)
	{
		for(const auto& layer : layers)
			if(layer->name == name)
				layers.remove(layer);
	}

	bool compare_layer_order(const shared_ptr<layer> rhs, const shared_ptr<layer> lhs)
	{
		return lhs->zorder < rhs->zorder;
	}

	void scene_manager::sort_layers()
	{
		//std::sort(begin(layers), end(layers)); // can we use this?
		layers.sort(compare_layer_order);
	}

	void scene_manager::update()
	{
		// Scene manager does not need updating
	}

	std::list<shared_ptr<layer>> scene_manager::get_scene_layers() const
	{
		return layers;
	}


	bool scene_manager::load_scene_file(const std::string& filename, shared_ptr<resource_manager> resource_admin)
	{
		if(current_scene_name == filename)
		{
			log_message(string("Scene already loaded. Skipping."));
			return true;
		}
		log_message("Loading scene: " + string(filename));
		
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

									// populate the list of game objects in this layer
									the_layer->game_objects.push_back(game_object_factory::get_instance().build_game_object(object, resource_admin, config));																
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

	string scene_manager::get_subscriber_name()
	{
		return "scene_manager";
	}

}
