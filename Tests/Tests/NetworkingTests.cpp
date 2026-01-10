
#include "pch.h"
#include <net/Networking.h>
#include "net/NetworkManager.h"
#include <thread>
#include <gtest/gtest.h>

#include "events/ControllerMoveEvent.h"
#include "events/EventManager.h"
#include "events/NetworkTrafficReceivedEvent.h"
#include "utils/Utils.h"

#include "events/NetworkPlayerJoinedEvent.h"
#include "file/SerializationManager.h"
#include "net/BitPacker.h"
#include "TestData.h"
#include "events/ReliableUdpAckPacketEvent.h"
#include "events/ReliableUdpPacketLossDetectedEvent.h"
#include "events/ReliableUdpPacketReceivedEvent.h"
#include "events/ReliableUdpPacketRttCalculatedEvent.h"
#include "net/GameServerConnectionFactory.h"
#include "net/GameClientConnectionFactory.h"
#include "net/ReliableUdp.h"
#include "net/ReliableUdpGameServerConnection.h"
#include "net/ReliableUdpProtocolManager.h"
#include <memory>
#include <thread>
#include <chrono>
#include <algorithm>
#include <cstring>


#include "events/EventFactory.h"
#include "net/GameClient.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

namespace gamelib
{

	class NetworkingTests : public testing::Test
	{
	public:

		const char* ListeningPort = "8080";
		const char* ServerAddress = "localhost";
		const char* ClientNickName = "Alice";
		const char* ServerNickName = "Bob";	
		const string ServerOrigin = ServerAddress + string(":") + ListeningPort;
		const Encoding WireEncoding = Encoding::json;

		std::thread ListeningThread;
		std::atomic<bool>  ServerListening{true};
		std::shared_ptr<GameClient> Client = nullptr;
		std::shared_ptr<GameServer> Server;

		bool serverReady = false;
		std::mutex mutex;
		bool condition{false};
		std::condition_variable conditional_variable;

		void StartNetworkServer(const std::shared_ptr<IGameServerConnection>& inGameServerConnection = nullptr)
		{
			// The main thread:

			std::cout << "Acquiring main lock\n";
			// Acquire mutex (server thread below will later wait on this lock)
			std::unique_lock<std::mutex> server_ready_lock(mutex);

			// The server thread:
			ListeningThread = std::thread(
				[this, inGameServerConnection]()
			{
				// Create a GameServer connection object specifying to listen on an address:port combination,
				// and which type of protocol to use.
				const auto defaultConnection =
					GameServerConnectionFactory::Create(false, ServerAddress, ListeningPort, false);

				// Create Game server object, which will use the defaultConnection details
				Server = std::make_shared<GameServer>(
					inGameServerConnection
					 ? inGameServerConnection
					 : defaultConnection);

				// Start the server effectively
				Server->Initialize();

				while (ServerListening)
				{
					if (!serverReady)
					{
						// Wait on the main thread to release mutex (WAIT) via wait
						std::unique_lock<std::mutex> server_ready_lock(mutex);

						// fulfil condition, that is blocking the main thread
						serverReady = true;
						conditional_variable.notify_all();
					}

					try
					{
						Server->Listen();
					}
					catch (std::exception& ex)
					{
						std::cout << "Exception occurred in Server listening thread: " << ex.what() << "\n";
					}
				}

				std::cout << "Server thread finished.\n";

				Server->Disconnect();

				std::cout << "Server disconnected.\n";
			});

			// Wait until the server is listening:

			while (!serverReady)
			{
				// Give up the lock in exchange for waiting for someone to notify the conditional variable
				conditional_variable.wait(server_ready_lock);
				// lock is re-acquired.
			}

			std::cout << "Server started.\n";
		}

		void SetUp() override
		{
			Networking::InitializeWinSock();
		}

		void TearDown() override
		{
			std::cout << "Test teardown starting...\n";

			// Break out of server's listening loop to stop it listening
			ServerListening = false;

			if (ListeningThread.joinable())
			{
				std::cout << "Waiting for server thread to finish...\n";
				ListeningThread.join();
			}

			std::cout << "Listening thread finished.\n";
			
			Client = nullptr;
			Server = nullptr;
			EventManager::Get()->Reset();
			EventManager::Get()->ClearSubscribers();
			std::cout << "Test teardown successful\n" ;
		}

		[[nodiscard]] std::tuple<vector<shared_ptr<Event>>, vector<shared_ptr<Event>>> PartitionEvents() const
		{
			auto events = EventManager::Get()->GetEvents();

			vector<shared_ptr<Event>> serverEmittedEvents;
			vector<shared_ptr<Event>> clientEmittedEvents;

			while(!events.empty())
			{
				const auto& event = events.front();
				if(event->Origin == ServerOrigin)
				{
					serverEmittedEvents.push_back(event);
				}
				else
				{
					clientEmittedEvents.push_back(event);
				}
				events.pop();
			}

			return {clientEmittedEvents, serverEmittedEvents};
		}

		[[nodiscard]] static shared_ptr<Event> FindNetworkTrafficReceivedEvent(std::vector<shared_ptr<Event>> events, const std::string &requiredMessageType, const std::string& trafficEventIdentifier)
		{
			const auto result = std::find_if(events.begin(), events.end(), 
		     [&](const shared_ptr<Event> & event)
		     {
		         if( event->Id == NetworkTrafficReceivedEventId)
		         {
		             const auto trafficReceivedEvent = To<NetworkTrafficReceivedEvent>(event);
					 const auto& messageType = gamelib::SerializationManager::GetMessageHeader(trafficReceivedEvent->Message).TheMessageType;
		             return messageType == requiredMessageType && trafficReceivedEvent->Identifier == trafficEventIdentifier;
		         }
		         return false;	
		     });
			if (result == events.end())
			{
				return nullptr;
			}
			return *result;
		}

		[[nodiscard]] static shared_ptr<Event> FindEmittedEvent(const std::vector<shared_ptr<Event>>& events, const EventId& eventId, bool findLast = false)
		{
			auto pred =  [&](const shared_ptr<Event> & event)
		     {
		         if( event->Id == eventId )
		         {
		             return true;
		         }
		         return false;	
		     };

			if(!findLast) 
				return *std::find_if(events.begin(), events.end(), pred);

			return *std::find_if(events.rbegin(), events.rend(), pred);
			
		}

		[[nodiscard]] static shared_ptr<NetworkPlayerJoinedEvent> FindPlayerJoinedEvent(const std::vector<shared_ptr<Event>>& events, const string& playerNick)
		{
			const auto result = std::find_if(events.begin(), events.end(), 
		     [&](const shared_ptr<Event> & event)
		     {
		         return event->Id == NetworkPlayerJoinedEventId && To<NetworkPlayerJoinedEvent>(event)->Player.GetNickName() == playerNick ? true : false;
		     });
			return To<NetworkPlayerJoinedEvent>(*result);
		}

		void EnsureNetworkJoinedEventEmittedByServer(const std::vector<shared_ptr<Event>>& events, const std::string &playerNick) const
		{
			const auto resultEvent = FindEmittedEvent(events, NetworkPlayerJoinedEventId);
			const auto joinedEvent = To<NetworkPlayerJoinedEvent>(resultEvent);
			EXPECT_TRUE(resultEvent != nullptr);
			EXPECT_EQ(joinedEvent->Origin, ServerOrigin);
			EXPECT_EQ(joinedEvent->Player.GetNickName(), playerNick);
		}

		static void EnsurePongTrafficReceivedByClient(const std::vector<shared_ptr<Event>>& clientEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin)
		{
			EXPECT_EQ(clientEmittedEvents.size(), 1) << "Expected the client to receive a pong in response to its ping request";

			const auto event = FindNetworkTrafficReceivedEvent(clientEmittedEvents, "pong", messageIdentifier);
			const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

			// The client emitted that it received traffic
			EXPECT_TRUE(trafficEvent != nullptr);
			EXPECT_EQ(trafficEvent->Identifier, messageIdentifier);
			EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);
		}

		static void EnsurePlayerPingTrafficReceivedByServer(const std::vector<shared_ptr<Event>>& serverEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin)
		{
			const auto event = FindNetworkTrafficReceivedEvent(serverEmittedEvents, "ping", messageIdentifier);
			const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

			// Ensure we got a ping response from the server
			EXPECT_TRUE(trafficEvent != nullptr);
			EXPECT_EQ(trafficEvent->BytesReceived, 147);
			EXPECT_EQ(trafficEvent->Identifier, messageIdentifier); // Ensure it came from the client
			EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);
			EXPECT_EQ(trafficEvent->Message, "{\"ages\": [1, 2, 3], \"eventType\": 1015, \"fish\": {\"Neemo\": \"Mathews\"}, \"isHappy\": true, \"messageType\": \"ping\", \"names\": [\"Stuart\", \"Jenny\", \"bruce\"]}");

		}

		static void EnsurePlayerJoinedTrafficReceivedByServer(const std::vector<shared_ptr<Event>>& serverEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin)
		{
			const auto event = FindNetworkTrafficReceivedEvent(serverEmittedEvents, "requestPlayerDetails", messageIdentifier);
			const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

			EXPECT_TRUE(trafficEvent != nullptr) << "Expected to find trafficReceivedEvent but as null";
			EXPECT_EQ(trafficEvent->Identifier, messageIdentifier); // Ensure it came from the client
			EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);

			stringstream expectedMessage;
			expectedMessage << R"({"messageType": "requestPlayerDetails", "nickname": )" << "\"" << messageIdentifier << "\"}";

			EXPECT_EQ(trafficEvent->Message,expectedMessage.str());
		}
		
	};

	TEST_F(NetworkingTests, TestConnectToServer)
	{
		StartNetworkServer();

		std::cout << "Starting client...\n";

		// Setup client
		Client = make_shared<GameClient>(ClientNickName, GameClientConnectionFactory::Create(false) /* using UDP*/);
		Client->Initialize();
		Client->Connect(Server);
		
		// Wait for the server to respond

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		Client->Read();

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();

		EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);
		
		EXPECT_EQ(serverEmittedEvents.size(), 2) << "Should be 1 joined traffic event, and 1 official player joined events - both emitted from server";
		EXPECT_EQ(clientEmittedEvents.size(), 0) << "No events expected to be emitted from client";
	}

	TEST_F(NetworkingTests, TestPing)
	{
		StartNetworkServer();

		std::cout << "Starting client...\n";

		// Setup client
		Client = make_shared<GameClient>(ClientNickName, GameClientConnectionFactory::Create(false) /* using UDP*/);
		Client->Initialize();
		Client->Connect(Server);
		
		// When pinging the game server...
		Client->PingGameServer(0);

		// Wait for the server to respond
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		// Read pong response
		Client->Read();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		// Collect events raised

		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
		
		EXPECT_EQ(clientEmittedEvents.size(), 1) << "Expected 1 response that the client received the pong traffic";
		
		EnsureNetworkJoinedEventEmittedByServer(serverEmittedEvents, ClientNickName);
		
		EXPECT_EQ(serverEmittedEvents.size(), 3) << "Should be 3 joined traffic events - emitted from server";

		EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);	
		EnsurePlayerPingTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);	
		EnsurePongTrafficReceivedByClient(clientEmittedEvents, "Game Server", ClientNickName);
	}

	TEST_F(NetworkingTests, TestCustomBinarySend)
	{
		#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

		constexpr static auto ReceiveBufferMaxElements = 300;
		constexpr static auto ReceiveBufferBytes = 300 * 32 / 8;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};

		// The game server connection will unpack/process our protocol messages
		const auto gameServerConnection = std::make_shared<ReliableUdpGameServerConnection>(ServerAddress, ListeningPort);

		// The game client connection will pack/send our protocol messages
		const auto gameClientConnection = std::make_shared<UdpConnectedNetworkSocket>();
		const auto client = std::make_shared<ReliableUdpProtocolManager>(gameClientConnection);

		client->Initialize();

		StartNetworkServer(gameServerConnection);

		std::cout << "Starting client...\n";

		// Establish connection and send public key
		client->Connect(ServerAddress, ListeningPort);

		sleep(1);
	
		// Receive ack for receiving pub key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		sleep(1);

		// Receive server's public key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// We will serialize bits 16-bits at a time
		uint16_t buffer[3]{};
		BitPacker bitPacker(buffer, 3);

		constexpr uint16_t value1 = 0;
		constexpr uint16_t value2 = 6;
		constexpr uint16_t value3 = 9;

		bitPacker.Pack(BITS_REQUIRED(0, 3), value1); // 00
		bitPacker.Pack(BITS_REQUIRED(0, value2), value2); //110
		bitPacker.Pack(BITS_REQUIRED(0, value3), value3); // 1001
		bitPacker.Flush();
		
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(buffer[0]), "0000000100111000");
				
		const auto bytesSend = client->Send((char*)buffer, bitPacker.TotalBitsPacked());

		// receive ack
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// Wait for the server to respond
		sleep(1);

		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
		
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
			FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true));

		const auto receivedBuffer = const_cast<char*>(trafficEvent->Message.c_str());
		
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00111000");
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00000001");

		BitfieldReader<char> bitFieldReader(receivedBuffer, trafficEvent->BytesReceived);

		EXPECT_EQ(bitFieldReader.ReadNext<char>(BITS_REQUIRED(0, 3)), value1);
		EXPECT_EQ(bitFieldReader.ReadNext<char>(BITS_REQUIRED(0, value2)), value2);
		EXPECT_EQ(bitFieldReader.ReadNext<char>(BITS_REQUIRED(0, value3)), value3);

	}

	TEST_F(NetworkingTests, TestBitPacketBinarySend)
	{
		constexpr static auto ReceiveBufferMaxElements = 300;
		constexpr static auto ReceiveBufferBytes = 300 * 32 / 8;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};

		// The game server connection will unpack/process our protocol messages
		const auto gameServerConnection = std::make_shared<ReliableUdpGameServerConnection>(ServerAddress, ListeningPort);

		// The game client connection will pack/send our protocol messages
		const auto gameClientConnection = std::make_shared<UdpConnectedNetworkSocket>();
		const auto client = std::make_shared<ReliableUdpProtocolManager>(gameClientConnection);

		client->Initialize();

		StartNetworkServer(gameServerConnection);

		std::cout << "Starting client...\n";

		// Establish connection and send public key
		client->Connect(ServerAddress, ListeningPort);

		// Receive ack for receiving pub key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// Receive server's public key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		TestData::TestNetworkPacket sendPacket{};

		sendPacket.NumElements = 3;
		sendPacket.Elements[0] = 10;
		sendPacket.Elements[1] = 9;
		sendPacket.Elements[2] = 1;
		
		uint16_t networkBuffer[3]{};

		BitPacker packer(networkBuffer, 3);

		sendPacket.Write(packer);
		packer.Finish();

		EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[0]), "0010010000101011");
		EXPECT_EQ(BitFiddler<uint16_t>::ToString(networkBuffer[1]), "0000000000000100");

		// 0000000000000100 001001_00001010_11

		client->Send((char*)networkBuffer, packer.TotalBitsPacked());

		// Receive ack 
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
		
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
			FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true));

		auto receivedBuffer = const_cast<char*>(trafficEvent->Message.c_str());
		
		BitfieldReader<const char> reader(receivedBuffer, trafficEvent->BytesReceived);

		TestData::TestNetworkPacket receivedPacket {};

		receivedPacket.Read(reader);

		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00101011");
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00100100");
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[2]), "00000100");
		EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[3]), "00000000");

		EXPECT_EQ(receivedPacket.NumElements, 3);
		EXPECT_EQ(receivedPacket.Elements[0], 10);
		EXPECT_EQ(receivedPacket.Elements[1], 9);
		EXPECT_EQ(receivedPacket.Elements[2], 1);
		
	}

	TEST_F(NetworkingTests, TestBitPacketBinarySendReadPayload)
	{
		constexpr static auto ReceiveBufferMaxElements = 300;
		constexpr static auto ReceiveBufferBytes = 300 * 32 / 8;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};

		// The game server connection will unpack/process our protocol messages
		const auto gameServerConnection = std::make_shared<ReliableUdpGameServerConnection>(ServerAddress, ListeningPort);

		// The game client connection will pack/send our protocol messages
		const auto gameClientConnection = std::make_shared<UdpConnectedNetworkSocket>();
		const auto client = std::make_shared<ReliableUdpProtocolManager>(gameClientConnection);

		client->Initialize();

		StartNetworkServer(gameServerConnection);

		std::cout << "Starting client...\n";

		// Establish connection and send public key
		client->Connect(ServerAddress, ListeningPort);

		// Receive ack for receiving pub key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// Receive server's public key
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// This is the packet we're going to send over the network
		TestData::TestNetworkPacket sendPacket {};

		// This is the packet we're going to populate with the bits received 
		TestData::TestNetworkPacket receivedPacket {};

		sendPacket.NumElements = 3;
		sendPacket.Elements[0] = 10;
		sendPacket.Elements[1] = 9;
		sendPacket.Elements[2] = 1;

		// Output buffer where we're going to output the serialized bits of the packet to
		uint16_t networkBuffer[3];

		// Hook the bit packer to output buffer, so that it packs to the expected place
		BitPacker packer(networkBuffer, 3);

		// Serialize to the buffer
		sendPacket.Write(packer); packer.Finish();

		// Bits should all be in the output buffer, lets send it
		client->Send((char*)networkBuffer, packer.TotalBitsPacked());

		// Receive ack
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		// Collect events from Event Manger to see what traffic was captured
		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();	
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
			FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true));

		// Extract the packet received into a receiveBuffer
		const char* receivedBuffer = trafficEvent->GetPayload();

		// Hook up bitfield reader to the received buffer
		BitfieldReader reader(receivedBuffer, trafficEvent->BytesReceived);

		// Read the packet's content from the receive buffer	
		receivedPacket.Read(reader);

		// Ensure what we send is what we received.
		EXPECT_EQ(receivedPacket.NumElements, 3);
		EXPECT_EQ(receivedPacket.Elements[0], 10);
		EXPECT_EQ(receivedPacket.Elements[1], 9);
		EXPECT_EQ(receivedPacket.Elements[2], 1);	
	}


	TEST_F(NetworkingTests, MultiPlayerJoinEventsEmittedOnConnect)
	{
		StartNetworkServer();

		std::cout << "Starting client...\n";

		const auto player1Nick = "Player1";
		const auto player2Nick = "Player2";
		const auto player3Nick = "Player3";

		// Setup client
		const auto client1 = make_shared<GameClient>(player1Nick, GameClientConnectionFactory::Create(false) /* using UDP*/);
		client1->Initialize();
		client1->Connect(Server);

		const auto client2 = make_shared<GameClient>(player2Nick, GameClientConnectionFactory::Create(false) /* using UDP*/);
		client2->Initialize();
		client2->Connect(Server);

		const auto client3 = make_shared<GameClient>(player3Nick, GameClientConnectionFactory::Create(false) /* using UDP*/);
		client3->Initialize();
		client3->Connect(Server);
		
		// Wait for the server to respond
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		client1->Read();
		client2->Read();
		client3->Read();
		
		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
		
		EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, player1Nick, ServerOrigin);	
		EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, player2Nick, ServerOrigin);
		EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, player3Nick, ServerOrigin);
		
		// Ensure Player Joined event emitted by server
		const auto candidateFind1 = FindPlayerJoinedEvent(serverEmittedEvents, player1Nick);
		const auto candidateFind2 = FindPlayerJoinedEvent(serverEmittedEvents, player2Nick);
		const auto candidateFind3 = FindPlayerJoinedEvent(serverEmittedEvents, player3Nick);

		EXPECT_EQ(candidateFind1->Player.GetNickName(), player1Nick);
		EXPECT_EQ(candidateFind2->Player.GetNickName(), player2Nick);
		EXPECT_EQ(candidateFind3->Player.GetNickName(), player3Nick);
		
		EXPECT_EQ(clientEmittedEvents.size(), 0) << "Expected no responses from server on connect, therefore no client events for traffic received";
		EXPECT_EQ(serverEmittedEvents.size(), 6) << "Should be 3 joined traffic events, and 3 official player joined events - both emitted from server";
	}


	TEST_F(NetworkingTests, ReliableUdpPacketLossResendTest)
	{
		constexpr static auto ReceiveBufferMaxElements = 300;
		constexpr static auto ReceiveBufferBytes = 300 * 32 / 8;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};

		// The game server connection will unpack/process our protocol messages
		const auto gameServerConnection = std::make_shared<ReliableUdpGameServerConnection>(ServerAddress, ListeningPort);

		// The game client connection will pack/send our protocol messages
		const auto gameClientConnection = std::make_shared<UdpConnectedNetworkSocket>();
		const auto reliableUdpProtocolManager = std::make_shared<ReliableUdpProtocolManager>(gameClientConnection);
		reliableUdpProtocolManager->Initialize();

		StartNetworkServer(gameServerConnection);

		std::cout << "Starting client...\n";

		auto data1 = "There can be only one.";
		auto data2 = "There can be only two.";
		auto data3 = "There can be only three.";

		const PacketDatum packet1(false, data1);
		const PacketDatum packet2(false, data2);
		const PacketDatum packet3(false, data3);

		// Establish connection and send public key
		reliableUdpProtocolManager->Connect(ServerAddress, ListeningPort);

		sleep(1);

		// Receive ack for receiving pub key
		reliableUdpProtocolManager->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		sleep(1);

		// Receive server's public key
		reliableUdpProtocolManager->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		sleep(1);

		// Tests: send data reliably with aggregated message support if not acknowledgment received
		
		reliableUdpProtocolManager->Send(data1, static_cast<int>(strlen(data1)));
		reliableUdpProtocolManager->Send(data2, static_cast<int>(strlen(data2)));
		reliableUdpProtocolManager->Send(data3, static_cast<int>(strlen(data3)));
			
		// Wait for the server to respond
		sleep(1);
			
		// Collect events from Event Manger to see what traffic was captured
		const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();

		// Expected server events
		
		EXPECT_EQ(serverEmittedEvents[0]->Id, ReliableUdpPacketReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[1]->Id, NetworkTrafficReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[2]->Id, ReliableUdpAckPacketEventId); 
		EXPECT_EQ(serverEmittedEvents[3]->Id, ReliableUdpAckPacketEventId); 
		EXPECT_EQ(serverEmittedEvents[4]->Id, ReliableUdpPacketRttCalculatedEventId);
		EXPECT_EQ(serverEmittedEvents[5]->Id, ReliableUdpPacketReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[6]->Id, NetworkTrafficReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[7]->Id, NetworkTrafficReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[8]->Id, ReliableUdpAckPacketEventId); 
		EXPECT_EQ(serverEmittedEvents[9]->Id, ReliableUdpPacketReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[10]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[11]->Id, NetworkTrafficReceivedEventId); 
		EXPECT_EQ(serverEmittedEvents[12]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[13]->Id, ReliableUdpAckPacketEventId);
		EXPECT_EQ(serverEmittedEvents[14]->Id, ReliableUdpPacketReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[15]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[16]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[17]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[18]->Id, NetworkTrafficReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[19]->Id, ReliableUdpAckPacketEventId);
			
		EXPECT_EQ(serverEmittedEvents.size(), 20);

		const int countData1 =
    std::count_if(serverEmittedEvents.begin(),
                  serverEmittedEvents.end(),
                  [data1](const std::shared_ptr<Event>& event) {
			if(event->Id == NetworkTrafficReceivedEventId)
			{
				const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);
				const auto result =  strcmp(trafficEvent->GetPayload(), data1) == 0;
				return result;
			}
			return false;
                  });

		const int countData2 =
		    std::count_if(
			serverEmittedEvents.begin(),
			serverEmittedEvents.end(),
			[data2](const std::shared_ptr<Event>& event)
			{
			    if (event->Id == NetworkTrafficReceivedEventId)
			    {
				const auto trafficEvent =
				    To<NetworkTrafficReceivedEvent>(event);

				return std::strcmp(trafficEvent->GetPayload(), data2) == 0;
			    }
			    return false;
			}
		    );


		const int countData3 =
		    std::count_if(
			serverEmittedEvents.begin(),
			serverEmittedEvents.end(),
			[data3](const std::shared_ptr<Event>& event)
			{
			    if (event->Id == NetworkTrafficReceivedEventId)
			    {
				const auto trafficEvent =
				    To<NetworkTrafficReceivedEvent>(event);

				return std::strcmp(trafficEvent->GetPayload(), data3) == 0;
			    }
			    return false;
			}
		    );

		EXPECT_EQ(countData1, 3); // we expect data1 to be sent once, with 2 retries
		EXPECT_EQ(countData2, 2); // we expect data2 to be sent once, with 1 retry
		EXPECT_EQ(countData3, 1); // we expect data3 to be sent once


		// Expected client events
		EXPECT_EQ(clientEmittedEvents[0]->Id, ReliableUdpAckPacketEventId);
		EXPECT_EQ(clientEmittedEvents[1]->Id, ReliableUdpPacketRttCalculatedEventId);
		EXPECT_EQ(clientEmittedEvents[2]->Id, ReliableUdpPacketReceivedEventId);
		EXPECT_EQ(clientEmittedEvents[3]->Id, ReliableUdpAckPacketEventId);
		EXPECT_EQ(clientEmittedEvents[4]->Id, ReliableUdpPacketLossDetectedEventId);
		EXPECT_EQ(clientEmittedEvents[5]->Id, ReliableUdpPacketLossDetectedEventId);
		EXPECT_EQ(clientEmittedEvents[6]->Id, ReliableUdpPacketLossDetectedEventId);

		EXPECT_EQ(clientEmittedEvents.size(), 7);
	}

	TEST_F(NetworkingTests, ReliableUdpRoutineConversation)
	{

		constexpr static auto ReceiveBufferMaxElements = 300;
		constexpr static auto ReceiveBufferBytes = 300 * 32 / 8;
		uint32_t readBuffer[ReceiveBufferMaxElements]{};

		BitfieldReader<uint32_t> reader(readBuffer, ReceiveBufferBytes);
		Message receivedMessage;

		// The game server connection will unpack/process our protocol messages
		const auto gameServerConnection = std::make_shared<ReliableUdpGameServerConnection>(ServerAddress, ListeningPort);

		// The game client connection will pack/send our protocol messages
		const auto gameClientConnection = std::make_shared<UdpConnectedNetworkSocket>();
		const auto client = std::make_shared<ReliableUdpProtocolManager>(gameClientConnection);
		client->Initialize();
		
		StartNetworkServer(gameServerConnection);

		std::cout << "Starting client...\n";
				
		auto thereCanBeOnlyOne = "There can be only one.";
		auto thereCanBeOnlyTwo = "There can be only two.";
		auto thereCanBeOnlyThree = "There can be only three.";

		std::tuple<vector<shared_ptr<Event>>, vector<shared_ptr<Event>>> allEvents;
		vector<shared_ptr<Event>> serverEmittedEvents;
		vector<shared_ptr<Event>> clientEmittedEvents;

		// ReSharper disable once CppInconsistentNaming
		auto ReadReceivedMessageAndAssertType = [&reader, &receivedMessage](MessageType expectedMessageType)
		{
			reader.Reset();
			receivedMessage.Read(reader);
			EXPECT_EQ(receivedMessage.Header.messageType, expectedMessageType);
		};

		const PacketDatum packet1(false, thereCanBeOnlyOne);
		const PacketDatum packet2(false, thereCanBeOnlyTwo);
		const PacketDatum packet3(false, thereCanBeOnlyThree);

		////----
		cout << "Test: Establish connection and send public key" << '\n';
		client->Connect(ServerAddress, ListeningPort);
								
		cout << "Test: Receive ack for receiving pub key" << '\n';
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		ReadReceivedMessageAndAssertType(MessageType::Ack); // got the ack back
		allEvents = PartitionEvents();
		serverEmittedEvents = std::get<1>(allEvents);

		EXPECT_EQ(serverEmittedEvents[0]->Id, ReliableUdpPacketReceivedEventId); // Data received: client's public key
		EXPECT_EQ(serverEmittedEvents[1]->Id, NetworkTrafficReceivedEventId); // Report data received: public key
		EXPECT_EQ(serverEmittedEvents[2]->Id, ReliableUdpAckPacketEventId); // sent ack for having received data (public key data)

		clientEmittedEvents = std::get<0>(allEvents);

		EXPECT_EQ(clientEmittedEvents[0]->Id, ReliableUdpAckPacketEventId);   // Client receives ack from server (server got the clients public key)
		EXPECT_EQ(clientEmittedEvents[1]->Id, ReliableUdpPacketRttCalculatedEventId);		

		///----
		cout << "Test: Receive server's public key" << '\n';
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		ReadReceivedMessageAndAssertType(MessageType::SendPubKey); // got server's pub key

		allEvents = PartitionEvents();
		clientEmittedEvents = std::get<0>(allEvents);

		EXPECT_EQ(clientEmittedEvents[2]->Id, ReliableUdpPacketReceivedEventId); // Data received: public key
		EXPECT_EQ(clientEmittedEvents[3]->Id, ReliableUdpAckPacketEventId); // Send ack

		serverEmittedEvents = std::get<1>(allEvents);

		///----
		// Send data1 reliably with aggregated message support if not acknowledgment received
		
		cout << "Test: send data1 encrypted" << '\n';
		client->Send(thereCanBeOnlyOne, static_cast<int>(strlen(thereCanBeOnlyOne)));
						
		cout << "Test: Receive ack for data1" << '\n';
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		ReadReceivedMessageAndAssertType(MessageType::Ack);
		allEvents = PartitionEvents();

		serverEmittedEvents = std::get<1>(allEvents);
		EXPECT_EQ(serverEmittedEvents[3]->Id, ReliableUdpAckPacketEventId); // receive ack
		EXPECT_EQ(serverEmittedEvents[4]->Id, ReliableUdpPacketRttCalculatedEventId);
		EXPECT_EQ(serverEmittedEvents[5]->Id, ReliableUdpPacketReceivedEventId); // receive data

		clientEmittedEvents = std::get<0>(allEvents);

		EXPECT_EQ(clientEmittedEvents[4]->Id, ReliableUdpPacketLossDetectedEventId); // sent aggregated message
		EXPECT_EQ(clientEmittedEvents[5]->Id, ReliableUdpAckPacketEventId); // received ack for what was sent to server (data1)
		EXPECT_EQ(clientEmittedEvents[6]->Id, ReliableUdpPacketRttCalculatedEventId);
			

		///----
		cout << "Test: Send encrypted data2" << '\n';
		client->Send(thereCanBeOnlyTwo, static_cast<int>(strlen(thereCanBeOnlyTwo)));
						
		cout << "Test: Receive ack for data2" << '\n';
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		ReadReceivedMessageAndAssertType(MessageType::Ack);
		allEvents = PartitionEvents();
		clientEmittedEvents = std::get<0>(allEvents);

		EXPECT_EQ(clientEmittedEvents[7]->Id, ReliableUdpAckPacketEventId);
		EXPECT_EQ(clientEmittedEvents[8]->Id, ReliableUdpPacketRttCalculatedEventId);
		
		serverEmittedEvents = std::get<1>(allEvents);

		EXPECT_EQ(serverEmittedEvents[6]->Id, NetworkTrafficReceivedEventId); // ""
		EXPECT_EQ(serverEmittedEvents[7]->Id, NetworkTrafficReceivedEventId); // "There can be only one"
		EXPECT_EQ(serverEmittedEvents[8]->Id, ReliableUdpAckPacketEventId);

		EXPECT_EQ(serverEmittedEvents[9]->Id, ReliableUdpPacketReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[10]->Id, NetworkTrafficReceivedEventId); // "There can be only two"
		EXPECT_EQ(serverEmittedEvents[11]->Id, ReliableUdpAckPacketEventId);

		///----
		cout << "Test: Send encrypted data3" << '\n';
		client->Send(thereCanBeOnlyThree, static_cast<int>(strlen(thereCanBeOnlyThree)));
						
		cout << "Test: Receive ack for data3" << '\n';
		client->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferBytes, 0);

		ReadReceivedMessageAndAssertType(MessageType::Ack);
		allEvents = PartitionEvents();
		clientEmittedEvents = std::get<0>(allEvents);

		EXPECT_EQ(clientEmittedEvents[9]->Id, ReliableUdpAckPacketEventId);
		EXPECT_EQ(clientEmittedEvents[10]->Id, ReliableUdpPacketRttCalculatedEventId);

		serverEmittedEvents = std::get<1>(allEvents);

		EXPECT_EQ(serverEmittedEvents[12]->Id, ReliableUdpPacketReceivedEventId);
		EXPECT_EQ(serverEmittedEvents[13]->Id, NetworkTrafficReceivedEventId); // "There can be only three"
		EXPECT_EQ(serverEmittedEvents[14]->Id, ReliableUdpAckPacketEventId);
			
		cout << "Test: Wait for the server to respond" << '\n';
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));


	}
}

