#pragma once
#include <random>
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

	inline const char* ToString(const Direction direction)
	{
		switch (direction)
		{
			case Direction::Up:   return "Up";
			case Direction::Down: return "Down";
			case Direction::Left: return "Left";
			case Direction::Right: return "Right";
			case Direction::None: return "None";
		}
		return nullptr;
	}

	inline Direction GetRandomDirection()
	{
		std::vector directions =  
			{
				Direction::Left,
				Direction::Right,
				Direction::Up,
				Direction::Down
			};


		std::vector<Direction> directionToSample;
			std::sample(begin(directions), end(directions), std::back_inserter(directionToSample), 1,
			            std::mt19937{std::random_device{}()});

			return directionToSample.front();
	}


	inline Direction FromDirectionString(const std::string& direction)
	{
		if(direction == ToString(Direction::Up))
		{
			return Direction::Up;
		}
		if (direction == ToString(Direction::Down))
		{
			return Direction::Down;
		}
		if (direction == ToString(Direction::Left))
		{
			return Direction::Left;
		}
		if (direction == ToString(Direction::Right))
		{
			return Direction::Right;
		}
		return Direction::None;
	}
}
