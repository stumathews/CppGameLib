#include "Room.h"
#include "util/RectDebugging.h"
#include <events/player_moved_event.h>


using namespace std;

namespace gamelib
{		
	/// <summary>
	/// Handle Room events
	/// </summary>
	/// <param name="event"></param>
	/// <returns></returns>
	vector<shared_ptr<Event>> Room::HandleEvent(const std::shared_ptr<Event> event)
	{	
		auto generatedEvents(GameObject::HandleEvent(event));

		// Handle when the player moves
		if(event->type == EventType::PlayerMovedEventType)
		{			
			const auto moved_event = std::static_pointer_cast<player_moved_event>(event);

			// Extract player info 
			const auto player_component = moved_event->get_player_component();

			// Get player
			const auto player = player_component->the_player;

			// Dont handle player events
			if(GetGameObjectType() == object_type::Player)
			{
				return generatedEvents;
			}

			// Set if the player is in this room or not
			isPlayerWithinRoom = (player->x >= bounds.x && player->x < bounds.x + bounds.w) && 
				                (player->y >= bounds.y && player->y < bounds.y + bounds.h);
			
			// Update the player's knowledge of which room its in
			if(isPlayerWithinRoom)
			{
				player->SetRoom(number);
			}

			// Update square's knowledge of player's bounds for layer use
			playerBounds = 
			{
				player->x, 
				player->y, 
				player->GetWidth(), 
				player->GetHeight()  
			};
		}

		// If settings are reloaded, reload the room settings
		if(event->type == EventType::SettingsReloaded)
		{
			LoadSettings(settings_admin);			
		}
		
		return generatedEvents;
	}

	/// <summary>
	/// Reload room settings
	/// </summary>
	/// <param name="settings_admin"></param>
	void Room::LoadSettings(SettingsManager& settings_admin)
	{
		// Load game object settings
		GameObject::LoadSettings(settings_admin);	

		// Refetch Room settings
		fill = settings_admin.get_bool("room_fill", "enable");
	}

	/// <summary>
	/// Draw the room
	/// </summary>
	/// <param name="renderer"></param>
	void Room::Draw(SDL_Renderer* renderer)
	{
		// Allow base class to perform any common drawing operations
		GameObject::Draw(renderer);

		// black
		SDL_SetRenderDrawColor(renderer, 0, 0,0,0);
		
		auto& rect = GetABCDRectangle();
		const auto ax = rect.GetAx();
		const auto ay = rect.GetAy();
		const auto bx = rect.GetBx();
		const auto by = rect.GetBy();
		const auto cx = rect.GetCx();
		const auto cy = rect.GetCy();
		const auto dx = rect.GetDx();
		const auto dy = rect.GetDy();

		const auto have_top_wall = this->walls[0];
		const auto have_right_wall = this->walls[1];
		const auto have_bottom_wall = this->walls[2];
		const auto have_left_wall = this->walls[3];

		// Draw the walls as lines
		if (have_top_wall) 
			SDL_RenderDrawLine(renderer, ax, ay, bx, by);
		
		if (have_right_wall) 
			SDL_RenderDrawLine(renderer, bx, by, cx, cy);
		
		if (have_bottom_wall) 
			SDL_RenderDrawLine(renderer, cx, cy, dx, dy);
		
		if (have_left_wall) 
			SDL_RenderDrawLine(renderer, dx, dy, ax, ay);
		
		if(fill)
			DrawFilledRect(renderer, &bounds, { 255, 0 ,0 ,0});
		
		if(settings_admin.get_bool("global", "print_debugging_text"))
		{
		  RectDebugging::printInRect(renderer, GetTag(), &bounds, _resourceManager); 
		}
	}

	/// <summary>
	/// Get room's  ABCD rectangle
	/// </summary>
	/// <returns>The Rooms ABCD rectangle</returns>
	ABCDRectangle& Room::GetABCDRectangle()
	{
		return abcd;
	}

	Room::Room(int number, 
		int x, 
		int y, 
		int width, 
		int height, 
		ResourceManager& resourceManager, 
		SettingsManager& settings, 
		EventManager& eventManager, bool fill)
		: DrawableGameObject(x, y, true, settings, eventManager), 
		 _resourceManager(resourceManager), fill(fill), playerBounds({}),
		 top_room_index(0), right_room_index(0),  bottom_room_index(0), width(width), height(height), left_room_index(0)
	{
		// Bounds of this room
		this->bounds =
		{
			x,
			y, 
			width,
			height
		};

		this->width = width;
		this->height = height;
		this->number = number;

		// Room geometry helper
		this->abcd = ABCDRectangle(x, y, width, height);

		this->supportsMoveLogic = false;

		// All walls are present by default
		walls[0] = true;
		walls[1] = true;
		walls[2] = true;
		walls[3] = true;
	}

	/// <summary>
	/// Set Indexs of sorrounding rooms
	/// </summary>
	/// <param name="top_index"></param>
	/// <param name="right_index"></param>
	/// <param name="bottom_index"></param>
	/// <param name="left_index"></param>
	void Room::SetSoroundingRooms(const int top_index, const int right_index, const int bottom_index, const int left_index)
	{
		this->top_room_index = top_index;
		this->right_room_index = right_index;
		this->bottom_room_index = bottom_index;
		this->left_room_index = left_index;
	}

	int Room::GetNeighbourIndex(Side side) const
	{
		switch (side)
		{
		case Side::Top:
			return top_room_index;
			break;
		case Side::Right:
			return right_room_index;
			break;
		case Side::Bottom:
			return bottom_room_index;
			break;
			case Side::Left:
			return left_room_index;
			break;
		default:
			return -1;
		}
	}

	/// <summary>
	/// corner x-coordinate
	/// </summary>
	/// <returns></returns>
	int Room::GetX() const
	{
		return this->x;
	}

	/// <summary>
	/// cornder y-coorinate
	/// </summary>
	/// <returns></returns>
	int Room::GetY() const
	{
		return this->y;
	}

	/// <summary>
	/// Get room width
	/// </summary>
	/// <returns></returns>
	int Room::GetWidth() const
	{
		return width;
	}

	/// <summary>
	/// Get Room height
	/// </summary>
	/// <returns></returns>
	int Room::GetHeight() const
	{
		return height;
	}

	bool Room::IsWalled(Side wall)
	{
		return walls[(int)wall];
	}

	// Walls dont respont to frame updates yet
	void Room::Update()
	{
	}

	/// <summary>
	/// Remove a wall
	/// </summary>
	/// <param name="wall">wall index</param>
	void Room::RemoveWall(Side wall)
	{
		this->walls[(int)wall-1] = false;
	}

	/// <summary>
	/// Remove a wall
	/// </summary>
	/// <param name="wall">wall index</param>
	void Room::RemoveWallZeroBased(Side wall)
	{
		this->walls[(int)wall] = false;
	}

	void Room::ShouldRoomFill(bool fill_me) 
	{
		fill = fill_me; 
	}

	object_type Room::GetGameObjectType() 
	{
		return object_type::Room;
	}

	/// <summary>
	/// Get name
	/// </summary>
	/// <returns></returns>
	string Room::GetName()
	{
		return "Room";
	}
}