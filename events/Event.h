#pragma once
#include <string>
#include <memory>
#include <exceptions/EngineException.h>
#include <sstream>

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

	inline gamelib::EventType FromString(std::string eventType)
	{
		if(eventType == ToString(EventType::ControllerMoveEvent))
		{
			return EventType::ControllerMoveEvent;
		}
		else if (eventType == ToString(EventType::LevelChangedEventType))
		{
			return EventType::LevelChangedEventType;
		}
		else if (eventType == ToString(EventType::DoLogicUpdateEventType))
		{
			return EventType::DoLogicUpdateEventType;
		}
		else if (eventType == ToString(EventType::DoLogicUpdateEventType))
		{
			return EventType::DoLogicUpdateEventType;
		}
		else if (eventType == ToString(EventType::AddGameObjectToCurrentScene))
		{
			return EventType::AddGameObjectToCurrentScene;
		}
		else if (eventType == ToString(EventType::PlayerMovedEventType))
		{
			return EventType::PlayerMovedEventType;
		}
		else if (eventType == ToString(EventType::SceneLoaded))
		{
			return EventType::SceneLoaded;
		}
		else if (eventType == ToString(EventType::SettingsReloaded))
		{
			return EventType::SettingsReloaded;
		}
		else if (eventType == ToString(EventType::GenerateNewLevel))
		{
			return EventType::GenerateNewLevel;
		}
		else if (eventType == ToString(EventType::InvalidMove))
		{
			return EventType::InvalidMove;
		}
		else if (eventType == ToString(EventType::Fire))
		{
			return EventType::Fire;
		}
		else if (eventType == ToString(EventType::FetchedPickup))
		{
			return EventType::FetchedPickup;
		}
		else if (eventType == ToString(EventType::GameObject))
		{
			return EventType::GameObject;
		}
		else if (eventType == ToString(EventType::DrawCurrentScene))
		{
			return EventType::DrawCurrentScene;
		}
		else if (eventType == ToString(EventType::NetworkPlayerJoined))
		{
			return EventType::NetworkPlayerJoined;
		}
		else if (eventType == ToString(EventType::NetworkTrafficReceived))
		{
			return EventType::NetworkTrafficReceived;
		}
		else
		{
			std::stringstream message;
			message << "Unknown Event Type " << eventType;
			THROW(0,message.str(), "Event");
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



