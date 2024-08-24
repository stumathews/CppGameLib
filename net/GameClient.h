#pragma once
#include "GameServer.h"
#include <WinSock2.h>
#include <events/EventSubscriber.h>

#include "IConnectedNetworkSocket.h"
#include "ReliableUdp.h"
#include "file/SerializationManager.h"
#include "security/Security.h"
#include "time/PeriodicTimer.h"

namespace gamelib
{
	class IProtocolManager;
	class ReliableUdpProtocolManager;
	class EventManager;
	class Networking;
	class EventFactory;
	class SerializationManager;

	class GameClient final : public EventSubscriber
	{
	public:
		~GameClient() override;
		GameClient(const std::string& nickName, const std::shared_ptr<IConnectedNetworkSocket>& connection,
		           bool useReliableUdpProtocolManager = false, bool useEncryption = true, Encoding encoding = Encoding::Json);
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
		void PingGameServer(const unsigned long deltaMs) const;

		// sends and array of items of type T across the network 
		int SendBinary(uint16_t* array, size_t numBits) const;

		/// <summary>
		/// Listen for incoming traffic from the game server
		/// </summary>
		void Read(unsigned long deltaMs = 0);
		
	private:
		std::shared_ptr<GameServer> gameServer;

		std::shared_ptr<IProtocolManager> networkProtocolManager;		
		
		
		fd_set readfds{};
		bool isDisconnectedFromGameServer;

		// How long to wait for network data the arrive {0,0} means non-blocking
		timeval noDataTimeout{};
		
		std::string nickName;
		EventManager* eventManager;
		std::shared_ptr<SerializationManager> serializationManager;
		Networking* networking;
		EventFactory* eventFactory;
		bool useEncryption {true};
		bool isTcp{};
		Encoding encoding;


		// Inherited via EventSubscriber
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs) override;
		void SubscribeToGameEvents();
		void CheckSocketForTraffic(unsigned long deltaMs);
		void RaiseNetworkTrafficReceivedEvent(const char* buffer, const int bytesReceived);
		int InternalSend(const char* array, const size_t size, const unsigned long deltaMs) const;
		int InternalSend(const std::string& message, const unsigned long deltaMs) const;
		void ParseReceivedServerPayload(const char* buffer, const unsigned long deltaMs) const;
		std::string GetSubscriberName() override;
		constexpr static auto ReceiveBufferMaxElements = 300;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};
	};
}

