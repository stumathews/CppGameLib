#pragma once
#include <string>
#include <memory>

namespace gamelib
{
	enum class EventType
	{
		ControllerMoveEvent,
		LevelChangedEventType,
		DoLogicUpdateEventType,
		AddGameObjectToCurrentScene,
		PlayerMovedEventType,
		SceneLoaded,
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
		int eventId = 0;
		bool processed = false;
		EventType type;
		virtual EventType GetGameObjectType();
		virtual std::string ToString();
		virtual ~Event() = default;
	};

	std::string operator+(const std::string& str, const EventType type);	
}



