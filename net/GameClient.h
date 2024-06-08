#pragma once
#include "GameServer.h"
#include <file/Logger.h>
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
		GameClient(const std::string& nickName, bool isTcp);
		GameClient(const GameClient& other) = delete;
		GameClient(const GameClient&& other) = delete;
		const GameClient& operator=(const GameClient& other) = delete;
		const GameClient& operator=(const GameClient&& other) = delete;

		/// <summary>
		/// Initializes the Game client.
		/// </summary>
		void Initialize();
		
		// Make an initial connection to the game server. This will register a socket with the game server tat we can later use to talk to it
		void Connect(const std::shared_ptr<GameServer>& inGameServer);
		void SendPlayerDetails() const;
		void PingGameServer() const;

		// sends and array of items of type T across the network 
		int SendBinary(uint16_t* array, size_t numBits) const;

		/// <summary>
		/// Listen for incoming traffic from the game server
		/// </summary>
		void Read();
		
	private:
		std::shared_ptr<GameServer> gameServer;

		/// <summary>
		/// The socket the game client will use to communicate with the game server
		/// </summary>
		SOCKET clientSocketToGameSever;
		fd_set readfds{};
		bool isDisconnectedFromGameServer;

		// How long to wait for network data the arrive {0,0} means non-blocking
		timeval noDataTimeout{};

		// What is the maximum amount of data to read off the network.
		int readBufferLength;
		std::string nickName;
		EventManager* eventManager;
		SerializationManager* serializationManager;
		Networking* networking;
		EventFactory* eventFactory;
		bool isTcp;

		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		void SubscribeToGameEvents();
		void CheckSocketForTraffic();
		void RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived);
		int InternalSend(const std::string& message) const;
		int InternalSend(const char* array, size_t size) const;
		void ParseReceivedServerPayload(char  buffer[512]) const;
		std::string GetSubscriberName() override;
	};
}

