#pragma once

#include <string>
#include "NetworkEvent.h"
#include "Connection.h"
#include <memory>
#include <net/GameClient.h>
#include <net/GameServer.h>

namespace gamelib
{
	// Setups networking to listens for network events
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
		bool IsGameServer();
		bool Initialize();

		// Cannot copy an NetworkManager
		NetworkManager(NetworkManager const&) = delete;
	
		~NetworkManager();
				
		// Cannot assign to an NetworkManager
		void operator=(NetworkManager const&) = delete;

		// Listen for network traffic destined for this this game server or this client
		void Listen();
		void PingGameServer() const;

		std::shared_ptr<GameClient> Client = nullptr;
		std::shared_ptr<GameServer> Server = nullptr;
	};
}

