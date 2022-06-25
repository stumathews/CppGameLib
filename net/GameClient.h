#pragma once
#include "GameServer.h"
#include <common/Logger.h>
#include <WinSock2.h>
#include <events/EventSubscriber.h>


namespace gamelib
{
	class EventManager;
	class SerializationManager;
	class GameClient : public gamelib::EventSubscriber
	{
	public:
		~GameClient();
		GameClient();
		void Initialize();
		
		// Make an initial connection to the game server. This will register a socket with the game server tat we can later use to talk to it
		void Connect(std::shared_ptr<GameServer> gameServer);
		void PingGameServer();

		/// <summary>
		/// Listen for incomiing traffic from the game server
		/// </summary>
		void Listen();
		void CheckSocketForTraffic();
		void ParseReceivedServerPayload(char  buffer[512]);
		void RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived);
	private:
		std::shared_ptr<GameServer> gameServer;

		/// <summary>
		/// The socket the game client will use to communicate with the game server
		/// </summary>
		SOCKET clientSocketToGameSever;
		fd_set readfds;
		bool IsDiconnectedFromGameServer;

		// How long to wait for network data the arrive {0,0} means non-blocking
		struct timeval noDataTimeout;

		// What is the maximum amount of data to read off the network.
		int readBufferLength;
		std::string nickName;
		EventManager* eventManager;
		SerializationManager* serializationManager;

		// Inherited via EventSubscriber
		virtual std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt) override;
		virtual std::string GetSubscriberName() override;
	};
}

