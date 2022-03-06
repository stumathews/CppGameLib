#pragma once

#include <memory>
#include <vector>

#include "DrawingBase.h"
#include "events/Event.h"
#include "scene/Room.h"

namespace gamelib
{
	/// <summary>
	/// Player object
	/// </summary>
	class Player final : public DrawableGameObject
	{
		bool drawBox = false;
		EventManager& eventManager;
		Logger& gameLogger;
		bool be_verbose;
		
	public:
		
		Player(int x, int y, int w, int h, SettingsManager& settings, EventManager& eventManager, Logger& gameLogger);
		
		/// <summary>
		/// Load player settings
		/// </summary>
		/// <param name="settings"></param>
		void LoadSettings(SettingsManager& settings) override;
		
		/// <summary>
		/// Center player in room
		/// </summary>
		/// <param name="target_room"></param>
		void CenterPlayerInRoom(std::shared_ptr<Room> target_room);
		static int get_room_neighbour_index(int first_room_index, int last_room_index, int side, std::shared_ptr<Room> room);

		/// <summary>
		/// Handle Player events
		/// </summary>
		/// <param name="event"></param>
		/// <returns></returns>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event) override;

		/// <summary>
		/// Draw player
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;

		/// <summary>
		/// Get Name
		/// </summary>
		/// <returns></returns>
		std::string GetName() override;

		/// <summary>
		/// Get Game oibject type
		/// </summary>
		/// <returns></returns>
		object_type GetGameObjectType() override;

		/// <summary>
		/// Update player
		/// </summary>
		void Update() override;

		/// <summary>
		/// Set the player's room
		/// </summary>
		/// <param name="roomIndex"></param>
		void SetRoom(int roomIndex);

		/// <summary>
		/// Get players width
		/// </summary>
		/// <returns></returns>
		int GetWidth();

		/// <summary>
		/// Get player's height
		/// </summary>
		/// <returns></returns>
		int GetHeight();

	private:
		int width;
		int height;
		int within_room_index = 0;
	};
}

