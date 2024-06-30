
#include "pch.h"
#include <net/Networking.h>
#include "net/NetworkManager.h"
#include <thread>

#include "events/ControllerMoveEvent.h"
#include "events/EventManager.h"
#include "events/NetworkTrafficReceivedEvent.h"
#include "utils/Utils.h"

#include "events/NetworkPlayerJoinedEvent.h"
#include "events/PlayerMovedEvent.h"
#include "events/StartNetworkLevelEvent.h"
#include "file/SerializationManager.h"
#include "net/BitPacker.h"
#include "TestData.h"
#include "net/ReliableUdp.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace gamelib;


class NetworkingTests : public testing::Test
{
public:

	const char* ListeningPort = "8080";
	const char* ServerAddress = "localhost";
	const char* ClientNickName = "Alice";
	const char* ServerNickName = "Bob";	
	const string ServerOrigin = ServerAddress + string(":") + ListeningPort;

	void StartNetworkServer()
	{
		Networking::Get()->InitializeWinSock();
		ServerListening = true;
		ListeningThread = thread([&]()
		{
			// Make a new server connection
			Server = std::make_shared<GameServer>(ServerAddress, ListeningPort, false /* using UDP*/);
			Server->Initialize();

			// Wait for connections
			while (ServerListening)
			{
				Server->Listen();
			}

			// Were done
			Server->Disconnect();
		});

		// Wait for the server to be ready
		while(Server == nullptr)
		{
			Sleep(250);
		}
	}
	

	void SetUp() override
	{
	}

	void TearDown() override
	{
		// break out of server's listening loop to stop it listening
		ServerListening = false;

		// finish listening thread
		ListeningThread.join();		
		
		//Client = nullptr;
		//Server = nullptr;
		EventManager::Get()->Reset();
		EventManager::Get()->ClearSubscribers();
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

	[[nodiscard]] shared_ptr<Event> FindNetworkTrafficReceivedEvent(std::vector<shared_ptr<Event>> events, const std::string requiredMessageType, const std::string& trafficEventIdentifier) const
	{
		const auto result = std::find_if(events.begin(), events.end(), 
	     [&](const shared_ptr<Event> & event)
	     {
	         if( event->Id == NetworkTrafficReceivedEventId)
	         {
	             const auto trafficReceivedEvent = To<NetworkTrafficReceivedEvent>(event);
				 const auto& messageType = SerializationManager::Get()->GetMessageHeader(trafficReceivedEvent->Message).MessageType;
	             return messageType == requiredMessageType && trafficReceivedEvent->Identifier == trafficEventIdentifier;
	         }
	         return false;	
	     });
		return *result;
	}

	[[nodiscard]] shared_ptr<Event> FindEmittedEvent(const std::vector<shared_ptr<Event>>& events, const EventId& eventId, bool findLast = false) const
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

	[[nodiscard]] shared_ptr<NetworkPlayerJoinedEvent> FindPlayerJoinedEvent(const std::vector<shared_ptr<Event>>& events, const string& playerNick) const
	{
		const auto result = std::find_if(events.begin(), events.end(), 
	     [&](const shared_ptr<Event> & event)
	     {
	         return event->Id == NetworkPlayerJoinedEventId && To<NetworkPlayerJoinedEvent>(event)->Player.GetNickName() == playerNick ? true : false;
	     });
		return To<NetworkPlayerJoinedEvent>(*result);
	}

	void EnsureNetworkJoinedEventEmittedByServer(const std::vector<shared_ptr<Event>>& events, const std::string playerNick) const
	{
		const auto resultEvent = FindEmittedEvent(events, NetworkPlayerJoinedEventId);
		const auto joinedEvent = To<NetworkPlayerJoinedEvent>(resultEvent);
		EXPECT_TRUE(resultEvent != nullptr);
		EXPECT_EQ(joinedEvent->Origin, ServerOrigin);
		EXPECT_EQ(joinedEvent->Player.GetNickName(), playerNick);
	}
	
	void EnsurePongTrafficReceivedByClient(const std::vector<shared_ptr<Event>>& clientEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin) const
	{
		EXPECT_EQ(clientEmittedEvents.size(), 1) << "Expected the client to receive a pong in response to its ping request";

		const auto event = FindNetworkTrafficReceivedEvent(clientEmittedEvents, "pong", messageIdentifier);
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

		// The client emitted that it received traffic
		EXPECT_TRUE(trafficEvent != nullptr);
		EXPECT_EQ(trafficEvent->Identifier, messageIdentifier);
		EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);
	}
		
	void EnsurePlayerPingTrafficReceivedByServer(const std::vector<shared_ptr<Event>>& serverEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin) const
	{
		const auto event = FindNetworkTrafficReceivedEvent(serverEmittedEvents, "ping", messageIdentifier);
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

		// Ensure we got a ping response from the server
		EXPECT_TRUE(trafficEvent != nullptr);
		EXPECT_EQ(trafficEvent->BytesReceived, 143);
		EXPECT_EQ(trafficEvent->Identifier, messageIdentifier); // Ensure it came from the client
		EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);
		EXPECT_EQ(trafficEvent->Message, "{\"ages\": [1, 2, 3], \"eventType\": 1015, \"fish\": {\"yo\": \"sushi\"}, \"isHappy\": false, \"messageType\": \"ping\", \"names\": [\"Stuart\", \"Jenny\", \"bruce\"]}");

	}

	void EnsurePlayerJoinedTrafficReceivedByServer(const std::vector<shared_ptr<Event>>& serverEmittedEvents, const std::string& messageIdentifier, const std::string& expectedEventOrigin) const
	{
		const auto event = FindNetworkTrafficReceivedEvent(serverEmittedEvents, "requestPlayerDetails", messageIdentifier);
		const auto trafficEvent = To<NetworkTrafficReceivedEvent>(event);

		EXPECT_EQ(trafficEvent->Identifier, messageIdentifier); // Ensure it came from the client
		EXPECT_EQ(trafficEvent->Origin, expectedEventOrigin);

		stringstream expectedMessage;
		expectedMessage << R"({"messageType": "requestPlayerDetails", "nickname": )" << "\"" << messageIdentifier << "\"}";

		EXPECT_EQ(trafficEvent->Message,expectedMessage.str());
	}
	
	std::thread ListeningThread;
	bool ServerListening{};
	std::shared_ptr<GameClient> Client = nullptr;
	std::shared_ptr<GameServer> Server;
};

TEST_F(NetworkingTests, TestConnectToServer)
{
	StartNetworkServer();

	// Setup client
	Client = make_shared<GameClient>(ClientNickName, false /* using UDP*/);
	Client->Initialize();
	Client->Connect(Server);
	
	// Wait for the server to respond
	Sleep(1000);
	Client->Read();

	const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();

	EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);
	
	EXPECT_EQ(serverEmittedEvents.size(), 2) << "Should be 1 joined traffic event, and 1 official player joined events - both emitted from server";
	EXPECT_EQ(clientEmittedEvents.size(), 0) << "No events expected to be emitted from client";
}

TEST_F(NetworkingTests, TestPing)
{
	StartNetworkServer();

	// Setup client
	Client = make_shared<GameClient>(ClientNickName, false /* using UDP*/);
	Client->Initialize();
	Client->Connect(Server);
	
	// When pinging the game server...
	Client->PingGameServer();

	// Wait for the server to respond
	Sleep(1000);

	// Read pong response
	Client->Read();

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

	StartNetworkServer();

	Client = make_shared<GameClient>(ClientNickName, false /* using UDP*/);
	Client->Initialize();
	Client->Connect(Server);

	// We will serialize bits 16-bits at a time
	uint16_t buffer[3]{};
	BitPacker bitPacker(buffer, 3);

	constexpr uint8_t minBitsFor0 = BITS_REQUIRED(0, 3); // 2
	constexpr uint8_t minBitsFor6 = BITS_REQUIRED(0, 6); // 3
	constexpr uint8_t minBitsFor9 = BITS_REQUIRED(0, 9); // 4

	bitPacker.Pack(minBitsFor0, 0); // 00
	bitPacker.Pack(minBitsFor6, 6); //110
	bitPacker.Pack(minBitsFor9, 9); // 1001
	bitPacker.Flush();
	
	EXPECT_EQ(BitFiddler<uint16_t>::ToString(buffer[0]), "0000000100111000");
	
	const auto expectedBytesSent = ceil(static_cast<double>(bitPacker.TotalBitsPacked()) / static_cast<double>(8));
	
	const auto bytesSend = Client->SendBinary(buffer, bitPacker.TotalBitsPacked());

	// Wait for the server to respond
	Sleep(1000);

	const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
	
	const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
		FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true));

	const auto receivedBuffer = const_cast<char*>(trafficEvent->Message.c_str());
	
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00111000");
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00000001");

	BitfieldReader<char> bitFieldReader(receivedBuffer, trafficEvent->BytesReceived);

	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor0), 0);
	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor6), 6);
	EXPECT_EQ(bitFieldReader.ReadNext<char>(minBitsFor9), 9);

	EXPECT_EQ(bytesSend, expectedBytesSent);
}

TEST_F(NetworkingTests, TestBitPacketBinarySend)
{
	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	StartNetworkServer();

	Client = make_shared<GameClient>(ClientNickName, false /* using UDP*/);
	Client->Initialize();
	Client->Connect(Server);

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

	const auto bytesSend = Client->SendBinary(networkBuffer, packer.TotalBitsPacked());

	// Wait for the server to respond
	Sleep(1000);

	const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
	
	const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
		FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true));

	auto receivedBuffer = (char*)(trafficEvent->Message.c_str());
	
	BitfieldReader<const char> reader(receivedBuffer, trafficEvent->BytesReceived);

	TestData::TestNetworkPacket receviedPacket {};

	receviedPacket.Read(reader);

	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[0]), "00101011");
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[1]), "00100100");
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[2]), "00000100");
	EXPECT_EQ(BitFiddler<char>::ToString(receivedBuffer[3]), "00000000");

	EXPECT_EQ(receviedPacket.NumElements, 3);
	EXPECT_EQ(receviedPacket.Elements[0], 10);
	EXPECT_EQ(receviedPacket.Elements[1], 9);
	EXPECT_EQ(receviedPacket.Elements[2], 1);
	
}

TEST_F(NetworkingTests, TestBitPacketBinarySendReadPayload)
{
	#define BITS_REQUIRED( min, max ) gamelib::BitsRequired<min,max>::result

	StartNetworkServer();

	Client = make_shared<GameClient>(ClientNickName, false /* using UDP*/);
	Client->Initialize();
	Client->Connect(Server);

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
	Client->SendBinary(networkBuffer, packer.TotalBitsPacked());

	// Wait for the server to respond
	Sleep(10);

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

	const auto player1Nick = "Player1";
	const auto player2Nick = "Player2";
	const auto player3Nick = "Player3";

	// Setup client
	const auto client1 = make_shared<GameClient>(player1Nick, false /* using UDP*/);
	client1->Initialize();
	client1->Connect(Server);

	const auto client2 = make_shared<GameClient>(player2Nick, false /* using UDP*/);
	client2->Initialize();
	client2->Connect(Server);

	const auto client3 = make_shared<GameClient>(player3Nick, false /* using UDP*/);
	client3->Initialize();
	client3->Connect(Server);
	
	// Wait for the server to respond
	Sleep(1000);

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

TEST_F(NetworkingTests, ReliableUdpTest)
{

	StartNetworkServer();

	const auto player1Nick = "Player1";

	// Setup client
	const auto client1 = make_shared<GameClient>(player1Nick, false /* using UDP*/);
	client1->Initialize();
	client1->Connect(Server);

	ReliableUdp reliableUdp;
	const PacketDatum packet(false, "There can be only one.");
	reliableUdp.Send(packet);
	reliableUdp.Send(packet);
	reliableUdp.Send(packet);
	
	Message* lastMessage = reliableUdp.Send(packet);
	lastMessage->Header.LastAckedSequence = 44;
	lastMessage->Header.LastAckedBits = 99;
		
	uint32_t buffer[5] {};
	BitPacker bitPacker(buffer, 5);

	// write last message out into buffer (binpacked)
	lastMessage->Write(bitPacker);
	
	// read out the last message from the buffer (binpacked)
	BitfieldReader reader(buffer, 5);
	
	Message message3;
	message3.Read(reader);	

	EXPECT_EQ(message3.Header.Sequence, lastMessage->Header.Sequence);
	EXPECT_EQ(message3.Header.LastAckedSequence, lastMessage->Header.LastAckedSequence);
	EXPECT_EQ(message3.Header.LastAckedBits, lastMessage->Header.LastAckedBits);

}

TEST_F(NetworkingTests, ReliableUdpTest2)
{

	Message receivedPacket {};
	StartNetworkServer();

	const auto player1Nick = "Player1";
	
	const auto client1 = make_shared<GameClient>(player1Nick, false);
	client1->Initialize();
	client1->Connect(Server);

	ReliableUdp reliableUdp;

	const PacketDatum packet(false, "There can be only one.");

	// Send as bitpacked binary
	auto message1 = reliableUdp.Send(client1, packet);
	auto message2 = reliableUdp.Send(client1, packet);
	auto message3 = reliableUdp.Send(client1, packet);
	
	// Wait for the server to respond
	Sleep(10);

	// Collect events from Event Manger to see what traffic was captured
	const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();	
	const auto trafficEvent = To<NetworkTrafficReceivedEvent>(
		FindEmittedEvent(serverEmittedEvents, NetworkTrafficReceivedEventId, true)); // <-- find only message 3 (last message)

	// Extract the packet received into a receiveBuffer
	const char* receivedBuffer = trafficEvent->GetPayload();

	// Hook up bitfield reader to the received buffer
	BitfieldReader reader((uint32_t*)receivedBuffer, trafficEvent->BytesReceived);

	// Read the packet's content from the receive buffer	
	receivedPacket.Read(reader);

	// Ensure what we send is what we received.
	EXPECT_EQ(receivedPacket.Header.Sequence, message3->Header.Sequence);	
	EXPECT_EQ(receivedPacket.Header.LastAckedBits, message3->Header.LastAckedBits);	
	EXPECT_EQ(receivedPacket.Header.LastAckedSequence, message3->Header.LastAckedSequence);	

}