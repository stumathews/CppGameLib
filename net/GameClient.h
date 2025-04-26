#pragma once
#include "GameServer.h"
#include <WinSock2.h>
#include <events/EventSubscriber.h>
#include <TestMe.h>
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

	/**
	 * \brief A game client connects to a game server via a network protocol and sends/received network instructions
	 */
	class GameClient final : public EventSubscriber
	{
	public:
		~GameClient() override;

		GameClient(const std::string& nickName, const std::shared_ptr<IConnectedNetworkSocket>& connection,
		           bool useReliableUdpProtocolManager = false, bool useEncryption = true,
		           Encoding encoding = Encoding::json, bool sendClientEventsToServer = true);
		GameClient(const GameClient& other) = delete;
		GameClient(const GameClient&& other) = delete;
		const GameClient& operator=(const GameClient& other) = delete;
		const GameClient& operator=(const GameClient&& other) = delete;

		void SetupProtocolManager(const std::shared_ptr<IConnectedNetworkSocket>& connection,
			bool useReliableUdpProtocolManager, bool useEncryption);

		/// <summary>
		/// Initializes the Game client.
		/// </summary>
		void Initialize();
		
		// Make an initial connection to the game server. This will register a socket with the game server tat we can later use to talk to it
		void Connect(const std::shared_ptr<GameServer>& inGameServer);

		// Game client operations:
			
		void PingGameServer(unsigned long deltaMs) const;

		// Listen for incoming traffic from the game server - see NetworkManager
		void Read(unsigned long deltaMs = 0);
		
	private:
		FRIEND_TEST(NetworkingTests, TestCustomBinarySend);
		FRIEND_TEST(NetworkingTests, TestBitPacketBinarySend);
		FRIEND_TEST(NetworkingTests, TestBitPacketBinarySendReadPayload);		

		// Sends and array of items of type T across the network - Used mainly for testing
		int SendBinary(uint16_t* array, size_t numBits) const;

		std::shared_ptr<GameServer> gameServer;
		bool sendClientEventsToServer;

		void SendPlayerDetails() const;

		// Interface to working with the networking protocol
		std::shared_ptr<IProtocolManager> networkProtocolManager;		

		// Network file descriptor set used to track if data is waiting on the network socket
		fd_set readfds{};

		bool isDisconnectedFromGameServer;

		// How long to wait for network data the arrive {0,0} means non-blocking
		timeval noDataTimeout{};

		// Client's nick name or identifier
		std::string nickName;
		EventManager* eventManager;

		// Helps create messages to send to the server
		std::shared_ptr<SerializationManager> serializationManager;

		// Raw networking functionality the client can use
		Networking* networking;

		// Events that the client can create for the game
		EventFactory* eventFactory;

		// Use TCP or UDP
		bool isTcp{};

		// Type of format to send data, eg. XML, JSON, Bit-packed etc.
		Encoding encoding;

		// Subscribe to other game events that occur
		void SubscribeToGameEvents();
		
		// Here is where we handle subscribed events
		std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event>& evt, unsigned long deltaMs) override;
				
		std::string GetSubscriberName() override;

		// Network receive buffer - where data received on the network is temporary stored before using it in the client
		constexpr static auto ReceiveBufferMaxElements = 300;
		uint32_t readBuffer[ReceiveBufferMaxElements]{}; // 300 x 32bits = 150kb

		// Check if there is any data sent by the game server waiting for client to read
		void CheckSocketForTraffic(unsigned long deltaMs);

		// Raise traffic received event
		void RaiseNetworkTrafficReceivedEvent(const char* buffer, int bytesReceived);

		// Internal send
		int InternalSend(const char* array, size_t size, unsigned long deltaMs) const;
		[[nodiscard]] int InternalSend(const std::string& message, unsigned long deltaMs) const;

		// Parse the network data sent by the server to the client. Likely using the serialization manager to deserialize data
		void ParseReceivedServerPayload(const char* buffer, unsigned long deltaMs) const;
	};
}

