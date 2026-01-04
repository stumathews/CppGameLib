#pragma once
#ifndef EVENTNUMBERS_H
#define EVENTNUMBERS_H

namespace gamelib
{
	// These are embedded in Events as the primary Id
	enum EventNumbers
	{
		Unknown = 1000,
		ControllerMove,
		SceneChangedEventType,
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
		SubscriberEventHandled,
		ReliableUdpPacketReceived,
		ReliableUdpCheckSumFailed,
		ReliableUdpPacketLossDetected,
		ReliableUdpAckPacket,
		ReliableUdpPacketRttCalculated
	};
}

#endif