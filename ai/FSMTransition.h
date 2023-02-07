#pragma once
#include <functional>
#include <utility>

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
	// ReSharper disable once CppInconsistentNaming
	class FSMState;
	// ReSharper disable once CppInconsistentNaming
	class FSMTransition
	{
	public:
		explicit FSMTransition(std::function<bool()> isValid = nullptr, std::function<FSMState*()> getNextState = nullptr, std::function<void()> onTransition = nullptr)
		: IsValid(std::move(isValid)), GetNextState(std::move(getNextState)), OnTransition(std::move(onTransition)) {  }
		std::function<bool()> IsValid;
		std::function<FSMState*()> GetNextState;
		std::function<void()> OnTransition;
	};
}
