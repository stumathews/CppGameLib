#pragma once

#include <memory>
#include <vector>

#include "DrawingBase.h"
#include "events/Event.h"
#include "scene/Room.h"

namespace gamelib
{
	
	class Player final : public DrawingBase, public std::enable_shared_from_this<Player>
	{
		bool draw_box = false;
		std::shared_ptr<event_manager> event_admin;
		
	public:
		int width, height;
		int within_room_index = 0;
		Player(int x, int y, int w, int h, std::shared_ptr<settings_manager> settings, std::shared_ptr<event_manager> event_admin);
		void load_settings(std::shared_ptr<settings_manager> settings) override;
		void center_player_in_room(std::shared_ptr<Room> target_room);
		static int get_room_neighbour_index(int first_room_index, int last_room_index, int side, std::shared_ptr<Room> room);
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void draw(SDL_Renderer* renderer) override;
		std::string get_identifier() override;
		object_type get_type() override { return object_type::player; }

		void update() override;
	};
}

