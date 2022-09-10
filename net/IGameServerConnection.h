#pragma once
#include <string>
namespace gamelib
{
	class IGameServerConnection
	{
	public:
		
		// Perform initialization
		virtual void Initialize() = 0;

		// Create listening socket
		virtual void Create() = 0;
		virtual void CheckForPlayerTraffic() = 0;

		// Listen for, read and then handle network traffic
		virtual void Listen() = 0;

		// Radiate duplicate serialized event to all connected players
		virtual void SendEventToAllPlayers(std::string serializedEvent) = 0;
	};
}
