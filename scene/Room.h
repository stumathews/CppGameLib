#pragma once
#include <SDL.h>
#include "ABCDRectangle.h"
#include "objects/DrawingBase.h"
#include "objects/GameObject.h"

namespace gamelib
{
	class ResourceManager;
	
	enum class Side
	{
		Top,
		Right,
		Bottom,
		Left
	};

	/// <summary>
	/// Conceptual room object
	/// </summary>
	class Room : public DrawableGameObject
	{	
	public:		
		
		/// <summary>
		/// Create a room
		/// </summary>
		Room(int number, 
			int x,
			int y, 
			int width, 
			int height, 
			ResourceManager& resource_admin, 
			SettingsManager& settings_admin, 
			EventManager& eventManager, 
			bool fill = false);
		
		
		/// <summary>
		/// Get Sorrounding rooms
		/// </summary>
		void SetSoroundingRooms(const int top_index, const int right_index, const int bottom_index,
		                             const int left_index);

		int GetNeighbourIndex(Side index) const;

	    /// <summary>
	    /// Get X coord
	    /// </summary>
	    /// <returns></returns>
	    int GetX() const;

	    /// <summary>
	    /// Get Y coord
	    /// </summary>
	    /// <returns></returns>
	    int GetY() const;

	    /// <summary>
	    /// Get Width
	    /// </summary>
	    /// <returns></returns>
	    int GetWidth() const;

	    /// <summary>
	    /// Get Heigh
	    /// </summary>
	    /// <returns></returns>
	    int GetHeight() const;

		/// <summary>
		/// IsWalled (zero based)
		/// </summary>
		/// <param name="wall"></param>
		/// <returns></returns>
		bool IsWalled(Side wall);

		/// <summary>
		/// Remove wall
		/// </summary>
		void RemoveWall(Side wall);

		/// <summary>
		/// Remove wall
		/// </summary>
		void RemoveWallZeroBased(Side wall);
		
		/// <summary>
		/// Should fill room?
		/// </summary>
		void ShouldRoomFill(bool fill_me = false);

		/// <summary>
		/// Provide Room type
		/// </summary>
		object_type GetGameObjectType() override;

		/// <summary>
		/// Name
		/// </summary>
		/// <returns></returns>
		std::string GetName() override;
		
		/// <summary>
		/// Handle room events
		/// </summary>
		/// <param name="event"></param>
		/// <returns></returns>
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> event) override;

		/// <summary>
		/// Load room settings
		/// </summary>
		/// <param name="settings_admin"></param>
		void LoadSettings(SettingsManager& settings_admin) override;		 

		/// <summary>
		/// Draw room
		/// </summary>
		/// <param name="renderer"></param>
		void Draw(SDL_Renderer* renderer) override;

	    /// <summary>
	    /// Update room
	    /// </summary>
	    void Update() override;

	protected:

		/// <summary>
		/// Each room as a room number
		/// </summary>
		int number;

		/// <summary>
		/// Each room as 4 possible walls
		/// </summary>
		bool walls[4];

		/// <summary>
		/// Each room's geometry is managed by a ABCD rectandle
		/// </summary>
		ABCDRectangle abcd;

	    /// <summary>
	    /// Get ABCD Rectangle
	    /// </summary>
	    /// <returns></returns>
	    ABCDRectangle& GetABCDRectangle();
		
		/// <summary>
		/// Index of the room above this one (room number)
		/// </summary>
		int top_room_index;

		/// <summary>
		/// Index of the room to the right of this one (room number)
		/// </summary>
		int right_room_index;

		/// <summary>
		/// Index of the room below this one (room number)
		/// </summary>
		int bottom_room_index; 

		/// <summary>
		/// Room to the left of this one (room number)
		/// </summary>
		int left_room_index;

		/// <summary>
		/// Width of this room
		/// </summary>
		int width;

		/// <summary>
		/// Height of this room
		/// </summary>
		int height;

	private:

		/// <summary>
		/// Resource manager
		/// </summary>
		ResourceManager& _resourceManager;

		/// <summary>
		/// Indication if the player is within this room at the moment
		/// </summary>
		bool isPlayerWithinRoom = false;

		/// <summary>
		/// indication if this room should be filled
		/// </summary>
		bool fill = false;

		/// <summary>
		/// The bounds of the player
		/// </summary>
		SDL_Rect playerBounds = {0};
	};
}


