#pragma once
#include "GameServer.h"
#include <common/Logger.h>
#include <WinSock2.h>

namespace gamelib
{
	class GameClient
	{
	public:
		~GameClient();
		GameClient();
		/// <summary>
		/// Connect to the game server
		/// </summary>
		/// <param name="gameServer"></param>
		void Connect(std::shared_ptr<GameServer> gameServer);
		void PingGameServer();

		/// <summary>
		/// Listen for incomiing traffic from the game server
		/// </summary>
		void Listen();
		void CheckForTraffic();
		void ParsePayload(char  buffer[512]);
		void RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived);
	private:
		std::shared_ptr<GameServer> gameServer;

		/// <summary>
		/// The socket the game client will use to communicate with the game server
		/// </summary>
		SOCKET clientSocket;
		fd_set readfds;
		bool IsDiconnectedFromGameServer;
		struct timeval noDataTimeout;
		int readBufferLength;
	};
}

