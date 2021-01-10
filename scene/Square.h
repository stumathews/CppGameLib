#pragma once
#include <SDL.h>
#include "abcd_rectangle.h"
#include "objects/game_object.h"

namespace gamelib
{
	class resource_manager;
	enum class square_role
	{
		Room, Pickup
	};

	class square : public game_object 
	{		
		std::shared_ptr<resource_manager> resource_admin;		
		bool is_player_in_square = false;
		bool fill = false;
		SDL_Rect player_bounds = {0};
	 protected:
		square_role role;
		int number;
		int width, height;
		bool walls[4]{};
		std::shared_ptr<abcd_rectangle> abcd;
	    [[nodiscard]] std::shared_ptr<abcd_rectangle> get_abcd() const;
		SDL_Rect bounds;
		int top_room_index, right_room_index, bottom_room_index, left_room_index = -1;
		void fill_me(SDL_Renderer* renderer, SDL_Color color) const;
	public:
		
		square(int number, int x, int y, int rw, int rh, std::shared_ptr<resource_manager> resource_admin,
		       bool fill = false, bool supports_move_logic = true, bool is_visible = true,
		       std::shared_ptr<settings_manager> settings_admin = std::make_shared<settings_manager>(),
		       square_role role = square_role::Room);

		object_type get_type() override { return object_type::square; }

		void set_adjacent_room_index(const int top_index, const int right_index, const int bottom_index,
		                             const int left_index);

		[[nodiscard]] int get_adjacent_index_for_wall(int index) const;
		[[nodiscard]] square_role get_square_role() const { return role;}

	    int get_x() const;
	    int get_y() const;
	    int get_w() const;
	    int get_h() const;

	    bool is_walled(int wall);
		bool is_walled_0_based(int wall);
		void remove_wall(int wall);
		std::string get_identifier() override;
		void set_fill(bool fill_me = false) { fill = fill_me;}
		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
		void load_settings(std::shared_ptr<settings_manager> settings_admin) override;
		 
		 void draw(SDL_Renderer* renderer) override;
	    virtual ~square() override;
	    void update() override;

		std::shared_ptr<settings_manager> settings_admin;
	};
}


