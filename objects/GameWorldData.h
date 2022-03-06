#pragma once
#include <memory>
#include <vector>

#include "GameObject.h"

class GameWorldData 
{
public:
	/*int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;*/
	
	/// <summary>
	/// Is the game done right now?
	/// </summary>
	bool IsGameDone = false;

	/// <summary>
	/// This is this a network game?
	/// </summary>
	bool IsNetworkGame = false;

	/// <summary>
	/// Can we draw right now?
	/// </summary>
	bool CanDraw = true;
		
	/// <summary>
	/// List of game objects
	/// </summary>
	std::vector<std::shared_ptr<gamelib::GameObject>> objects;
};

