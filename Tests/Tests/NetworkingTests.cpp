
#include "pch.h"
#include <net/Networking.h>
#include "net/NetworkManager.h"
#include <thread>

#include "events/EventManager.h"
#include "events/NetworkTrafficReceivedEvent.h"
#include "utils/Utils.h"

#pragma comment(lib, "ws2_32.lib")

using namespace std;
using namespace gamelib;

class NetworkingTests : public ::testing::Test, public EventSubscriber, public enable_shared_from_this<NetworkingTests>
{
public:

	const char* ListeningPort = "8080";
	const char* ServerAddress = "localhost";

	std::shared_ptr<NetworkTrafficReceivedEvent> NetTrafficReceivedEvent = nullptr;

	std::vector<std::shared_ptr<Event>> HandleEvent(const std::shared_ptr<Event> event, unsigned long deltaMs) override
	{
		if(event->Id == NetworkTrafficReceivedEventId)
		{
			NetTrafficReceivedEvent = To<NetworkTrafficReceivedEvent>(event);
		}
		return {};
	}

	std::string GetSubscriberName() override
	{
		return "NetworkingTests";
	}

	void StartNetworkServer()
	{
		Networking::Get()->InitializeWinSock();
		ServerListening = true;
		ListeningThread = thread([&]()
		{
			// Make a new server connection
			Server = std::make_shared<GameServer>(ServerAddress, ListeningPort);
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
		SubscribeToEvent(NetworkTrafficReceivedEventId);
	}

	void TearDown() override
	{
		// break out of server's listening loop to stop it listening
		ServerListening = false;

		// finish listening thread
		ListeningThread.join();

		// reset everything
		NetTrafficReceivedEvent = nullptr;
		Client = nullptr;
		Server = nullptr;
	}

	
	std::thread ListeningThread;
	bool ServerListening{};
	std::shared_ptr<GameClient> Client = nullptr;
	std::shared_ptr<GameServer> Server;
};

TEST_F(NetworkingTests, TestConnectToServer)
{
	EXPECT_TRUE(NetTrafficReceivedEvent == nullptr);

	StartNetworkServer();

	// Setup client
	Client = make_shared<GameClient>();
	Client->Initialize();
	Client->Connect(Server);
	
	// Wait for the server to respond
	Sleep(1000);

	// Collect events raised by the server, hopefully informing us that it got something
	EventManager::Get()->ProcessAllEvents();

	// Ensure we got a the player details response
	EXPECT_TRUE(NetTrafficReceivedEvent != nullptr);
	EXPECT_EQ(NetTrafficReceivedEvent->BytesReceived, 62);
	EXPECT_EQ(NetTrafficReceivedEvent->Identifier, "Charlie");
	EXPECT_EQ(NetTrafficReceivedEvent->Message, "{\"messageType\": \"requestPlayerDetails\", \"nickname\": \"Charlie\"}");

}

TEST_F(NetworkingTests, TestPingServer)
{
	EXPECT_TRUE(NetTrafficReceivedEvent == nullptr);

	StartNetworkServer();

	// Setup client
	Client = make_shared<GameClient>();
	Client->Initialize();
	Client->Connect(Server);

	// Process events (which we'll ignore)
	EventManager::Get()->ProcessAllEvents();
	NetTrafficReceivedEvent = nullptr;

	// When pinging the game server...
	Client->PingGameServer();

	// Wait for the server to respond
	Sleep(1000);

	// Collect events raised by the server, hopefully informing us that it got something
	EventManager::Get()->ProcessAllEvents();

	// Ensure we got a ping response
	EXPECT_TRUE(NetTrafficReceivedEvent != nullptr);
	EXPECT_EQ(NetTrafficReceivedEvent->BytesReceived, 143);
	EXPECT_EQ(NetTrafficReceivedEvent->Identifier, "Charlie");
	EXPECT_EQ(NetTrafficReceivedEvent->Message, "{\"ages\": [1, 2, 3], \"eventType\": 1015, \"fish\": {\"yo\": \"sushi\"}, \"isHappy\": false, \"messageType\": \"ping\", \"names\": [\"Stuart\", \"Jenny\", \"bruce\"]}");

}