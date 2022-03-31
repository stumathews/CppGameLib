#pragma once
#include <memory>
#include <vector>
#include "GameObject.h"

namespace gamelib
{
	class GameWorldData
	{
	public:
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

		std::vector<std::shared_ptr<gamelib::GameObject>>& GetGameObjects();

		std::shared_ptr<gamelib::GameObject> player;
	private:
		/// <summary>
		/// List of game objects
		/// </summary>
		std::vector<std::shared_ptr<gamelib::GameObject>> objects;
	};
}

