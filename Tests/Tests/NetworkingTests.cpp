
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
		
		Client = nullptr;
		Server = nullptr;
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
			const auto event = events.front();
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
	             const auto messageType = SerializationManager::Get()->GetMessageHeader(trafficReceivedEvent->Message).MessageType;
	             return messageType == requiredMessageType && trafficReceivedEvent->Identifier == trafficEventIdentifier;
	         }
	         return false;	
	     });
		return *result;
	}

	[[nodiscard]] shared_ptr<Event> FindEmittedEvent(const std::vector<shared_ptr<Event>>& events, const EventId& eventId) const
	{
		const auto result = std::find_if(events.begin(), events.end(), 
	     [&](const shared_ptr<Event> & event)
	     {
	         if( event->Id == eventId )
	         {
	             return true;
	         }
	         return false;	
	     });
		return *result;
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
	Client->Listen();

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
	Client->Listen();

	// Collect events raised

	const auto [clientEmittedEvents, serverEmittedEvents] = PartitionEvents();
	
	EXPECT_EQ(clientEmittedEvents.size(), 1) << "Expected 1 response that the client received the pong traffic";
	
	EnsureNetworkJoinedEventEmittedByServer(serverEmittedEvents, ClientNickName);
	
	EXPECT_EQ(serverEmittedEvents.size(), 3) << "Should be 3 joined traffic events - emitted from server";

	EnsurePlayerJoinedTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);	
	EnsurePlayerPingTrafficReceivedByServer(serverEmittedEvents, ClientNickName, ServerOrigin);	
	EnsurePongTrafficReceivedByClient(clientEmittedEvents, "Game Server", ClientNickName);
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

	client1->Listen();
	client2->Listen();
	client3->Listen();
	
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