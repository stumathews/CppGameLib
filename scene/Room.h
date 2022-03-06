#pragma once
#include <SDL.h>
#include "abcd_rectangle.h"
#include "objects/DrawingBase.h"
#include "objects/GameObject.h"

namespace gamelib
{
	class ResourceManager;
	
	class Room : public DrawingBase
	{		
		ResourceManager& resource_admin;		
		bool is_player_in_room = false;
		bool fill = false;
		SDL_Rect player_bounds = {0};

	protected:
		int number;
		bool walls[4]{};
		std::shared_ptr<abcd_rectangle> abcd;
	    [[nodiscard]] std::shared_ptr<abcd_rectangle> get_abcd() const;
		int top_room_index, right_room_index, bottom_room_index, left_room_index = -1;
		int width, height;
	public:		
		Room(int number, int x, int y, int rw, int rh, ResourceManager& resource_admin,
		       SettingsManager& settings_admin, EventManager& event_admin, bool fill = false);

		
		// Room functions
		void set_adjacent_room_index(const int top_index, const int right_index, const int bottom_index,
		                             const int left_index);

		[[nodiscard]] int get_adjacent_index_for_wall(int index) const;

	    int get_x() const;
	    int get_y() const;
	    int get_w() const;
	    int get_h() const;

		// Wall functions
	    bool is_walled(int wall);
		bool is_walled_0_based(int wall);
		void remove_wall(int wall);
		
		
		void set_fill(bool fill_me = false) { fill = fill_me;}

		object_type get_type() override { return object_type::room; }
		std::string get_identifier() override;
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void load_settings(SettingsManager& settings_admin) override;		 
		void draw(SDL_Renderer* renderer) override;
	    void update() override;
	};
}


