#pragma once
#include <functional>

/*
const std::function<coordinate<int>(Room, Player)> centerPlayerFunc = [](const Room& room, Player p)
	{
		auto const room_x_mid = room.GetX() + (room.GetWidth() / 2);
		auto const room_y_mid = room.GetY() + (room.GetHeight() / 2);
		auto const x = room_x_mid - p.width /2;
		auto const y = room_y_mid - p.height /2;
		return coordinate<int>(x, y);
	};
*/
namespace gamelib
{
	class FSMState;
	class FSMTransition
	{
	public:
		std::function<bool()> IsValid;
		std::function<FSMState*()> GetNextState;
		std::function<void()> OnTransition;
	};
}
