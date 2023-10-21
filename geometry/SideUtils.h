#pragma once
#include "character/Direction.h"
#include "Side.h"

namespace gamelib
{
	class SideUtils
	{
	public:
		// Turns a side enum into a loggable string
		static const char* SideToString(const Side side)
		{
			switch (side)
			{
				case Side::Top: return "Top";
				case Side::Right: return "Right";
				case Side::Bottom: return "Bottom";
				case Side::Left: return "Left";
			}
			return nullptr;
		}

		static Side GetOppositeSideForDirection(const Direction direction)
		{
			switch(direction)
			{
			case Direction::Up: return Side::Bottom;
			case Direction::Down: return Side::Top;
			case Direction::Left: return Side::Right;
			case Direction::Right: return Side::Left;
			}
		}

		static Side GetSideForDirection(const Direction direction)
		{
			switch(direction)
			{
			case Direction::Up: return Side::Top;
			case Direction::Down: return Side::Bottom;
			case Direction::Left: return Side::Left;
			case Direction::Right: return Side::Right;
			}
		}
	};
}
