#pragma once
#include <string>

namespace gamelib
{
	enum class EventType
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
		Fire,
		FetchedPickup,
		GameObject,
		DrawCurrentScene
	};

	class Event
	{
	public:
		explicit Event(EventType type, int event_id = 0);
		int event_id = 0;
		bool processed = false;
		EventType type;
		virtual EventType GetGameObjectType();
		virtual std::string to_str();
		virtual ~Event() = default;
	};

	std::string operator+(const std::string& str, const EventType type);
}



