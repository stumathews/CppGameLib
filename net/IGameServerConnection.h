#pragma once
#include <string>
namespace gamelib
{
	class IGameServerConnection
	{
	public:
		virtual void Initialize() = 0;
		virtual void CheckForPlayerTraffic() = 0;
		virtual void Listen() = 0;

		// Radiate duplicate serialized event to all connected players
		virtual void SendEventToAllPlayers(std::string serializedEvent) = 0;
	};
}
