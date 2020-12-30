#pragma once
#include <memory>
#include <vector>

#include "game_object.h"

class game_world_data 
{
public:
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	
	bool is_game_done = false;
	bool is_network_game = false;
	bool can_render = true;
		
	// List of game objects
	std::vector<std::shared_ptr<gamelib::game_object>> game_objects;
};

