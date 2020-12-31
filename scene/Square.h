#pragma once
#include <SDL.h>
#include "abcd_rectangle.h"
#include "objects/game_object.h"

namespace gamelib
{
	class resource_manager;

	class square : public game_object 
	{
		std::shared_ptr<resource_manager> resource_admin;		
		bool is_player_in_room = false;
		bool fill = false;
		SDL_Rect player_bounds;
	 protected:
		int number;
		int width, height;
		bool walls[4]{};
		std::shared_ptr<abcd_rectangle> abcd;
	    [[nodiscard]] std::shared_ptr<abcd_rectangle> get_abcd() const;
		SDL_Rect bounds;
		int top_room_index, right_room_index, bottom_room_index, left_room_index = -1;
		void fill_me(SDL_Renderer* renderer) const;
	public: 
		square(int number, int x, int y, int rw, int rh, std::shared_ptr<resource_manager> resource_admin,  bool fill = false, bool supports_move_logic = true, bool is_visible = true, std::shared_ptr<settings_manager> settings_admin = std::make_shared<settings_manager>());

		object_type get_type() override { return object_type::square; }
		void set_adjacent_room_index(int top_index, int right_index, int bottom_index, int left_index)
		{
			this->top_room_index = top_index;
			this->right_room_index = right_index;
			this->bottom_room_index = bottom_index;
			this->left_room_index = left_index;
		}

		int get_adjacent_index_for_wall(int index) const
		{
			switch(index)
			{
			case 0:
				return top_room_index;
				break;
			case 1:
				return right_room_index;
				break;
			case 2:
				return bottom_room_index;
				break;
			case 3:
				return left_room_index;
				break;
			default:
				return -1;
			}
		}
		
	    int get_x() const;
	    int get_y() const;
	    int get_w() const;
	    int get_h() const;

	    bool is_walled(int wall);
		 bool is_walled_0_based(int wall);
		 void remove_wall(int wall);
		std::string get_identifier() override;

		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void load_settings(std::shared_ptr<settings_manager> settings_admin) override;
		 
		 void draw(SDL_Renderer* renderer) override;
	    virtual ~square() override;
	    void update() override;

		std::shared_ptr<settings_manager> settings_admin;
	};
}


