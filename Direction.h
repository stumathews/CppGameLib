#pragma once
#include <string>

namespace gamelib 
{
	/// <summary>
	/// Direction
	/// </summary>
	enum class Direction 
	{ 
		Up, 
		Down,
		Left, 
		Right,
		None
	};

	inline const char* ToString(Direction direction)
	{
		switch (direction)
		{
			case Direction::Up:   return "Up";
			case Direction::Down: return "Down";
			case Direction::Left: return "Left";
			case Direction::Right: return "Right";
			case Direction::None: return "None";
			default:      
				return "[Unknown Direction]";
		}
	}

	
	inline const Direction FromDirectionString(std::string direction)
	{
		if(direction == ToString(Direction::Up))
		{
			return Direction::Up;
		}
		else if (direction == ToString(Direction::Down))
		{
			return Direction::Down;
		}
		else if (direction == ToString(Direction::Left))
		{
			return Direction::Left;
		}
		else if (direction == ToString(Direction::Right))
		{
			return Direction::Right;
		}
		else
		{
			return Direction::None;
		}	
	}
}
