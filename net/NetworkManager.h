#pragma once

#include <string>
#include <memory>
#include "file/SerializationManager.h"


namespace gamelib
{
	class GameServer;
	class GameClient;
	/**
	 * \brief Serves as a top-level abstraction for all networking capabilities in either a game server or a game client
	 * Setups networking to listens for network events
	 */
	class NetworkManager
	{
	protected:
			static NetworkManager* instance;
			bool isGameServer;
			std::string gameServerAddress;
			std::string gameServerPort;
			int maxPlayers;
			std::string nickName;
			bool isTcp;
			bool useEncryption;
			Encoding encoding;

		public:

		static NetworkManager* Get();

		NetworkManager();
		
		~NetworkManager();

		[[nodiscard]] bool IsGameServer() const;

		bool Initialize();

		// Cannot copy an NetworkManager
		NetworkManager(NetworkManager const&) = delete;
				
		// Cannot assign to an NetworkManager
		void operator=(NetworkManager const&) = delete;

		// Listen for network traffic destined for this this game server or this client
		void Listen(unsigned long deltaMs) const;

		// Ping game server if NetworkManager is operating in a game client
		void PingGameServer(unsigned long deltaMs) const;

		// Represents the client if NetworkManager is operating as a client
		std::shared_ptr<GameClient> Client = nullptr;

		// Represents a game server if NetworkManager is operating as a game server
		std::shared_ptr<GameServer> Server = nullptr;
	};
}

