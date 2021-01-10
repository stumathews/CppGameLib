#pragma once
#include <string>

namespace gamelib
{
	enum class event_type
	{
		PositionChangeEventType,
		LevelChangedEventType,
		DoLogicUpdateEventType,
		AddGameObjectToCurrentScene,
		PlayerMovedEventType,
		scene_loaded,
		SettingsReloaded,
		GenerateNewLevel,
		InvalidMove,
		Fire
	};

	class event
	{
	public:
		explicit event(event_type type, int event_id = 0);
		int event_id = 0;
		bool processed = false;
		event_type type;
		virtual event_type get_type();
		virtual std::string to_str();
		virtual ~event() = default;
	};

	std::string operator+(const std::string& str, const event_type type);
}



