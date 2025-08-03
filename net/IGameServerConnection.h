#pragma once
#include <string>

namespace gamelib
{
	// Interface for a game server's connection to a client
	class IGameServerConnection
	{
	public:
		virtual ~IGameServerConnection() = default;

		// Perform initialization
		virtual void Initialize() = 0;

		// Create listening socket
		virtual void Create() = 0;
		virtual void CheckForPlayerTraffic(unsigned long deltaMs) = 0;

		// Listen for, read and then handle network traffic
		virtual void Listen(unsigned long deltaMs) = 0;

		// Radiate duplicate serialized event to all connected players
		virtual void SendEventToAllPlayers(std::string serializedEvent) = 0;

		// Closes the game server's listening socket
		virtual void Disconnect() = 0;
	};
}
