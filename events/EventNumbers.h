#pragma once

namespace gamelib
{
	// These are embedded in Events as the primary Id
	enum EventNumbers
	{
		Unknown = 1000,
		ControllerMove,
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
		GameObjectTypeEvent,
		DrawCurrentScene,
		NetworkPlayerJoined,
		NetworkTrafficReceived,
		StartNetworkLevel,
		UpdateProcesses,
		GameWon,
	};


	 

	
}