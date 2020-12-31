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
	public:
		int within_room_index = 0;
		void set_room_within(size_t index){ within_room_index = index; }
		player(int x, int y, int w, int h, std::shared_ptr<resource_manager> resource_admin, std::shared_ptr<settings_manager> settings);
		void load_settings(std::shared_ptr<settings_manager> settings) override;
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void draw(SDL_Renderer* renderer) override;
		std::string get_identifier() override;
		object_type get_type() override { return object_type::player; }
	
	};
}

