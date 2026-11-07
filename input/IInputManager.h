#pragma once
#include <list>

#include "time/DeltaTime.h"

namespace gamelib
{
	class IInputManager
	{
	public:
		IInputManager() = default;
		virtual ~IInputManager() = default;
		IInputManager(const IInputManager& other) = delete;
		IInputManager(const IInputManager&& other) = delete;
		IInputManager& operator=(const IInputManager& other) = delete;
		IInputManager& operator=(const IInputManager&& other) = delete;

		virtual void Sample(const gamelib::DeltaTime* deltaTime) = 0;
	};
}

