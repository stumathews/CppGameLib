#include "game_object_factory.h"
#include "game_object.h"
#include "Sprite.h"
#include "common/static_config.h"
#include "resource/resource_manager.h"
#include "scene/Square.h"

using namespace tinyxml2;
using namespace std;

namespace gamelib
{
	game_object_factory& game_object_factory::get_instance()
	{
		static game_object_factory instance;
		return instance;
	}

	game_object_factory::game_object_factory() = default;
	game_object_factory::~game_object_factory() = default;

	shared_ptr<game_object> game_object_factory::build_game_object(XMLElement * scene_object_xml, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin) const
	{
		uint red = 0, green = 0, blue = 0;	
		uint x = 0, y = 0;
		auto visible = false, color_key_enabled = false;
		shared_ptr<graphic_resource> resource;
		
		auto empty_game_object = shared_ptr<game_object>(nullptr);

		for(const auto* scene_obj_att = scene_object_xml->FirstAttribute(); scene_obj_att; scene_obj_att = scene_obj_att->Next()) 
		{
			std::string detail_name(scene_obj_att->Name());
			std::string detail_value(scene_obj_att->Value());		
			
			if(detail_name == "resourceId") 
			{
				const auto resource_id = detail_value.c_str();
				auto asset_data = resource_admin->get(std::atoi(resource_id));
				if(asset_data != nullptr)
				{
					if(!asset_data->type._Equal("graphic"))
						throw exception(("Cannot load non graphic resource: " + asset_data->name + " type=" + asset_data->type).c_str());

					if(asset_data == nullptr)
						throw exception(("Could not load resource meta data for resource id:" + detail_value).c_str());

					resource = std::dynamic_pointer_cast<graphic_resource>(asset_data);
				}
			}

			// Object's initial x position
			if(detail_name == "posx")
			{
				x = std::stoi(detail_value);
				continue;
			}

			// Object initial visibility setting
			if(detail_name == "visible")
			{
				visible = detail_value._Equal("true") ? true : false;			
			}

			// object initial y position
			if(detail_name == "posy")
			{
				y = stoi(detail_value);
				continue;
			}

			// object's associated colour key
			if(detail_name == "colourKey") 
			{
				color_key_enabled = detail_value._Equal("true") ? true : false;			
				continue;
			}

			// Objects color
			if(detail_name._Equal("r")) {
				red = stoi(detail_value);
				continue;
			}
			if(detail_name == "g") {
				green = std::stoi(detail_value);
				continue;
			}
			if(detail_name == "b")
				blue = std::stoi(detail_value);
		}
			
		return initialize_game_object(empty_game_object, x, y, visible, resource, color_key_enabled, red, green, blue, resource_admin, settings_admin);		
	}

	std::shared_ptr<game_object>& game_object_factory::initialize_game_object(std::shared_ptr<game_object>& game_object, uint x, uint y, bool is_visible, std::shared_ptr<graphic_resource>& resource, const bool color_key_enabled, const uint& red, const uint& green, const uint& blue, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings_admin) const
	{	
		if(resource == nullptr)
		{			
			// Square
			game_object = std::make_shared<square>(x, y, settings_admin->get_int("global", "square_width"), resource_admin);
			return game_object;		
		}

		if( (red < 0 || red > 255)  || (blue < 0 || blue > 255) || (green < 0 || green > 255) )
			throw exception("Invalid color values when constructing game object");
		
		if(resource->is_animated)
		{
			// Sprite		
			game_object = std::make_shared<sprite>(x, y, settings_admin->get_int("global","sprite_width"),
				resource->num_key_frames, static_config::frames_per_row,
				static_config::frames_per_column, resource->key_frame_width,
				resource->key_frame_height, is_visible, settings_admin);
			std::dynamic_pointer_cast<sprite>(game_object)->play();
		} 
		else
		{
			// 2D Sprite (no animation)
			game_object = std::make_shared<sprite>(x, y, 1, 1, static_config::frames_per_row, static_config::frames_per_column, resource->key_frame_width, 1, is_visible, settings_admin);	
		}
		
		game_object->set_graphic_resource(resource);
		game_object->is_color_key_enabled = color_key_enabled;
		game_object->is_visible = is_visible;
		

		if (game_object->is_color_key_enabled)
			game_object->set_color_key(red, green, blue);

		return game_object;
	}
	}


