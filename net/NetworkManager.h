#pragma once

#include <string>
#include "NetworkEvent.h"
#include "Connection.h"
#include <memory>
//#include <winsock2.h>
//#include <ws2tcpip.h>
//#include <iphlpapi.h>
#include <net/Networking.h>
#include <net/GameClient.h>
#include <net/GameServer.h>

namespace gamelib
{
	class NetworkManager
	{
	protected:
			static NetworkManager* Instance;
	private:
			std::shared_ptr<Connection> remote;
			bool isGameServer;
			std::string gameServerAddress;
			std::string gameServerPort;
			int maxPlayers;

	public:
		static NetworkManager* Get();
		NetworkManager();
		bool Initialize();

		// Cannot copy an NetworkManager
		NetworkManager(NetworkManager const&) = delete;
	
		~NetworkManager();
		
		// Cannot assign to an NetworkManager
		void operator=(NetworkManager const&) = delete;

		NetworkEvent GetNetworkEvent();

		std::shared_ptr<GameClient> Client = nullptr;
		std::shared_ptr<GameServer> Server = nullptr;
	};
}

