#pragma once
#include <memory>

namespace gamelib
{
	class GameObject;

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

		std::weak_ptr<GameObject> player;

		[[nodiscard]] bool IsSinglePlayerGame() const { return !IsNetworkGame; }

		[[nodiscard]] bool IsMultiPlayerGame() const { return IsNetworkGame; }

		unsigned long ElapsedGameTime;
	};
}

