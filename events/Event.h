#pragma once
#include <string>
#include <sstream>

namespace gamelib
{
	enum class EventType
	{
		Unknown,
		ControllerMoveEvent,
		LevelChangedEventType,
		UpdateAllGameObjectsEventType,
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
		NetworkTrafficReceived,
		StartNetworkLevel,
		UpdateProcesses,
		GameWon,
	};

	inline const char* ToString(const EventType type)
	{
		switch (type)
		{

			case EventType::ControllerMoveEvent: return "ControllerMoveEvent";
			case EventType::LevelChangedEventType: return "LevelChangedEventType";
			case EventType::UpdateAllGameObjectsEventType: return "DoLogicUpdateEventType";
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
			case EventType::StartNetworkLevel: return "StartNetworkPlayerLevel";
			case EventType::Unknown: return "Unknown";
			case EventType::GameWon: return "GameWon";
			case EventType::UpdateProcesses: return "UpdateProcesses";
		}
		return nullptr;
	}

	inline EventType FromString(const std::string& eventType)
	{
		if(eventType == ToString(EventType::ControllerMoveEvent))
		{
			return EventType::ControllerMoveEvent;
		}
		if (eventType == ToString(EventType::LevelChangedEventType))
		{
			return EventType::LevelChangedEventType;
		}
		if (eventType == ToString(EventType::UpdateAllGameObjectsEventType))
		{
			return EventType::UpdateAllGameObjectsEventType;
		}
		if (eventType == ToString(EventType::AddGameObjectToCurrentScene))
		{
			return EventType::AddGameObjectToCurrentScene;
		}
		if (eventType == ToString(EventType::PlayerMovedEventType))
		{
			return EventType::PlayerMovedEventType;
		}
		if (eventType == ToString(EventType::SceneLoaded))
		{
			return EventType::SceneLoaded;
		}
		if (eventType == ToString(EventType::SettingsReloaded))
		{
			return EventType::SettingsReloaded;
		}
		if (eventType == ToString(EventType::GenerateNewLevel))
		{
			return EventType::GenerateNewLevel;
		}
		if (eventType == ToString(EventType::InvalidMove))
		{
			return EventType::InvalidMove;
		}
		if (eventType == ToString(EventType::Fire))
		{
			return EventType::Fire;
		}
		if (eventType == ToString(EventType::FetchedPickup))
		{
			return EventType::FetchedPickup;
		}
		if (eventType == ToString(EventType::GameObject))
		{
			return EventType::GameObject;
		}
		if (eventType == ToString(EventType::DrawCurrentScene))
		{
			return EventType::DrawCurrentScene;
		}
		if (eventType == ToString(EventType::NetworkPlayerJoined))
		{
			return EventType::NetworkPlayerJoined;
		}
		if (eventType == ToString(EventType::NetworkTrafficReceived))
		{
			return EventType::NetworkTrafficReceived;
		}
		if(eventType == ToString(EventType::StartNetworkLevel))
		{
			return EventType::StartNetworkLevel;
		}
		return EventType::Unknown;
	}

	class Event
	{
	public:
		explicit Event(EventType type);
		int Id = 0;
		bool Processed = false;
		EventType Type;
		std::string Origin;
		virtual EventType GetGameObjectType();
		virtual std::string ToString();
		virtual ~Event();
	private:
		// Game Object Id counter
		static int lastEventId;
	};

	std::string operator+(const std::string& str, EventType type);	
}



