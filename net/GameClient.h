#pragma once
#include "GameServer.h"
#include <common/Logger.h>
#include <WinSock2.h>
#include <events/EventSubscriber.h>


namespace gamelib
{
	class EventManager;
	class Networking;
	class EventFactory;
	class SerializationManager;

	class GameClient final : public EventSubscriber
	{
	public:
		~GameClient() override;
		GameClient();

		/// <summary>
		/// Initializes the Game client.
		/// </summary>
		void Initialize();

		void SubscribeToGameEvents();
		
		// Make an initial connection to the game server. This will register a socket with the game server tat we can later use to talk to it
		void Connect(const std::shared_ptr<GameServer>& inGameServer);
		void SendPlayerDetails() const;
		void PingGameServer() const;

		/// <summary>
		/// Listen for incomiing traffic from the game server
		/// </summary>
		void Listen();
		void CheckSocketForTraffic();
		void ParseReceivedServerPayload(char  buffer[512]) const;
		void RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived) const;
	private:
		std::shared_ptr<GameServer> gameServer;

		/// <summary>
		/// The socket the game client will use to communicate with the game server
		/// </summary>
		SOCKET clientSocketToGameSever;
		fd_set readfds{};
		bool IsDisconnectedFromGameServer;

		// How long to wait for network data the arrive {0,0} means non-blocking
		timeval noDataTimeout{};

		// What is the maximum amount of data to read off the network.
		int readBufferLength;
		std::string nickName;
		EventManager* _eventManager;
		SerializationManager* serializationManager;
		Networking* networking;
		EventFactory* _eventFactory;
		bool isTcp;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs) override;
		std::string GetSubscriberName() override;
	};
}

