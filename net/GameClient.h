#pragma once
#include "GameServer.h"
#include <common/Logger.h>
#include <WinSock2.h>
#include <events/EventSubscriber.h>

namespace gamelib
{
	class GameClient : public gamelib::EventSubscriber
	{
	public:
		~GameClient();
		GameClient();
		void Initialize();
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
		void ParseReceivedPayload(char  buffer[512]);
		void RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived);
	private:
		std::shared_ptr<GameServer> gameServer;

		/// <summary>
		/// The socket the game client will use to communicate with the game server
		/// </summary>
		SOCKET clientSocketToGameSever;
		fd_set readfds;
		bool IsDiconnectedFromGameServer;
		struct timeval noDataTimeout;
		int readBufferLength;
		std::string nickName;

		// Inherited via EventSubscriber
		virtual std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt) override;
		virtual std::string GetSubscriberName() override;
	};
}

