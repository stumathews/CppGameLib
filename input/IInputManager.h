#pragma once
#include <list>

#include "events/Event.h"

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

		virtual void Sample(unsigned long deltaMs) = 0;
		virtual int GetSampleRatePerSec() = 0;
	};
}

