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
		DrawCurrentScene,
		NetworkPlayerJoined,
		NetworkTrafficReceived
	};

	inline const char* ToString(EventType type)
	{
		switch (type)
		{

			case EventType::ControllerMoveEvent: return "ControllerMoveEvent";
			case EventType::LevelChangedEventType: return "LevelChangedEventType";
			case EventType::DoLogicUpdateEventType: return "DoLogicUpdateEventType";
			case EventType::AddGameObjectToCurrentScene: return "AddGameObjectToCurrentScene";
			case EventType::PlayerMovedEventType: return "PlayerMovedEventType";
			case EventType::SceneLoaded: return "SceneLoaded";
			case EventType::SettingsReloaded: return "SettingsReloaded";
			case EventType::GenerateNewLevel: return "GenerateNewLevel";
			case EventType::InvalidMove: return "InvalidMove";
			case EventType::Fire: return "Fire";
			case EventType::FetchedPickup: return "FetchedPickup";
			case EventType::GameObject: return "GameObject";
			case EventType::DrawCurrentScene: return "DrawCurrentScene";
			case EventType::NetworkPlayerJoined: return "NetworkPlayerJoined";
			case EventType::NetworkTrafficReceived: return "NetworkPlayerTrafficReceived";
			default:      
				return "[Unknown EventType]";
		}
	}

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



