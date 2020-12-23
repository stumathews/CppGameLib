#include "LevelGenerator.h"
#include <stack>
#include <ctime>
using namespace std;

namespace gamelib
{
	vector<shared_ptr<square>> level_generator::generate_level( std::shared_ptr<resource_manager> resource_admin, shared_ptr<settings_manager> settings_admin)
	{
		srand(static_cast<unsigned>(time(nullptr)));

		const auto screen_width = static_config::screen_width;
		const auto screen_height = static_config::screen_height;	
		const auto square_width = static_config::square_width;
		const auto max_rows = screen_width/square_width;
		const auto max_columns = screen_height/square_width;
		
		vector<shared_ptr<square>> mazeGrid;
		stack<shared_ptr<square>> roomStack;

		/* Generate Rooms for the Maze */
		auto count = 0;
		for(auto y = 0; y < max_columns; y++)
		{
			for(auto x = 0; x < max_rows; x++)
			{
				auto support_move_logic = false;
				auto game_object = std::make_shared<square>(x * square_width, y * square_width, square_width, resource_admin, support_move_logic, false, true, settings_admin);				
				game_object->set_tag(std::to_string(count++));
				mazeGrid.push_back(game_object);			
			}
		}

		const auto total_rooms = static_cast<int>(mazeGrid.size());	
		
		/* Determine which faces/edges can be removed, based on the bounds of the grid i.e within rows x cols of board */
		for(auto i = 0; i < total_rooms; i++)
		{
			const auto next_index = i + 1;
			const auto prevIndex = i - 1;

			if(next_index >= total_rooms)
				break;

			const auto this_row = abs(i / max_columns);
			const auto last_column = (this_row+1 * max_columns)-1;
			const auto thisColumn = max_columns - (last_column-i);

			const auto room_above_index = i - max_columns;
			const auto room_below_index = i + max_columns;
			const auto room_left_index = i - 1;
			const auto room_right_index = i + 1;

			const auto can_remove_above = room_above_index >= 0;
			const auto can_remove_below = room_below_index < total_rooms;
			const auto can_remove_left = thisColumn-1 >= 1;
			const auto can_remove_right = thisColumn+1 <= max_columns;

			vector<int> removableSides;
			auto current_room = mazeGrid[i];
			auto next_room = mazeGrid[next_index];

			if(can_remove_above && current_room->is_walled(top_side) && mazeGrid[room_above_index]->is_walled(bottom_side))
				removableSides.push_back(top_side);
			if(can_remove_below  && current_room->is_walled(bottom_side) && mazeGrid[room_below_index]->is_walled(top_side))
				removableSides.push_back(bottom_side);
			if(can_remove_left  && current_room->is_walled(left_side) && mazeGrid[room_left_index]->is_walled(right_side))
				removableSides.push_back(left_side);
			if(can_remove_right  && current_room->is_walled(right_side) && mazeGrid[room_right_index]->is_walled(left_side))
				removableSides.push_back(right_side);

			const int rand_side_index = rand() % removableSides.size(); // Choose a random element wall to remove from possible choices
			const auto remove_sides_randomly = true;
			if(remove_sides_randomly)
			{
				switch(removableSides[rand_side_index])
				{
				case top_side:
					current_room->remove_wall(top_side);
					next_room->remove_wall(bottom_side);
					continue;
				case right_side:
					current_room->remove_wall(right_side);
					next_room->remove_wall(left_side);
					continue;
				case bottom_side:
					current_room->remove_wall(bottom_side);
					next_room->remove_wall(top_side);
					continue;
				case left_side:
					current_room->remove_wall(left_side);				
					auto prev = mazeGrid[prevIndex];
					prev->remove_wall(right_side);
					continue;
				}
			}
		}
		return mazeGrid;
	}
}
