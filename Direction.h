#pragma once

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
}
