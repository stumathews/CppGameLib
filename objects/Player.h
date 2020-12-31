#pragma once

#include <memory>
#include <vector>
#include "events/Event.h"
#include "scene/Square.h"

namespace gamelib
{
	
	class player final : public square, public std::enable_shared_from_this<player>
	{
		bool draw_box;
		std::shared_ptr<event_manager> event_admin;
	public:
		int within_room_index = 0;
		player(int x, int y, int w, int h, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> setting, std::shared_ptr<event_manager> event_admin);
		void load_settings(std::shared_ptr<settings_manager> settings) override;
		void center_player_in_room(std::shared_ptr<square> target_room);
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void draw(SDL_Renderer* renderer) override;
		std::string get_identifier() override;
		object_type get_type() override { return object_type::player; }
	
	};
}

