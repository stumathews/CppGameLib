#pragma once
#include <SDL.h>
#include "rect_details.h"
#include "objects/game_object.h"

namespace gamelib
{
	class resource_manager;

	class square : public game_object 
	{
	private:
		 std::shared_ptr<resource_manager> resource_admin;
	    int width;
	protected:
		bool walls[4]{};
		std::shared_ptr<rect_details> rect_details_;
	    [[nodiscard]] std::shared_ptr<rect_details> get_rect_details() const;
		int fill = true;
		SDL_Rect player_bounds_;
		SDL_Rect my_bounds_;
	public: 
		square(int x, int y, int rw, std::shared_ptr<resource_manager> resource_admin,  bool fill = false, bool supports_move_logic = true, bool is_visible = true);

	    int get_x() const;
	    int get_y() const;
	    int get_w() const;
	    int get_h() const;

	    bool is_walled(int wall);
	    void remove_wall(int wall);
		std::string get_identifier() override;

		std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
	    void draw(SDL_Renderer* renderer) override;
	    virtual ~square() override;
	    void update() override;
	};
}


