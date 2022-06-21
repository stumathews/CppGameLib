#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>
#include <events/PlayerMovedEvent.h>

using namespace json11;

namespace gamelib
{
	GameClient::GameClient()
	{
		// What is the maximum amount of data to read off the network.
		this->readBufferLength = 512;

		// How long to wait for network data the arrive {0,0} means non-blocking		
		this->noDataTimeout.tv_sec = 0;
		this->noDataTimeout.tv_usec = 0;		

		this->clientSocketToGameSever = -1;
		this->IsDiconnectedFromGameServer = true;
		this->readfds = {0};
	}

	void GameClient::Initialize()
	{
		// Read client nickname
		this->nickName  = SettingsManager::Get()->GetString("networking", "nickname");

		// We subscribe to some of our down events and send them to the game server...
		EventManager::Get()->SubscribeToEvent(gamelib::EventType::PlayerMovedEventType, this);
	}

	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;
		Logger::Get()->LogThis(message.str());

		// Make a connection to the game server.
		clientSocketToGameSever = Networking::Get()->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str());			
						
		if(clientSocketToGameSever)
		{
			this->IsDiconnectedFromGameServer = false;
		}
	}	

	void GameClient::Listen()
	{
		if(this->IsDiconnectedFromGameServer)
		{
			return;
		}

		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment

		// Clear the list of sockets that we are listening for/on
		FD_ZERO(&readfds);
		
		// Add it to the list of file descriptors to listen for readability
		FD_SET(clientSocketToGameSever, &readfds);
				
		// Check monitored sockets for incoming 'readable' data
		auto dataIsAvailable = select(maxSockets, &readfds, NULL, NULL, &noDataTimeout) > 0;

		if (dataIsAvailable)
		{
			CheckForTraffic();
		}
	}

	void GameClient::CheckForTraffic()
	{
		// New connection on listening socket?
		if (FD_ISSET(clientSocketToGameSever, &readfds))
		{
			const int DEFAULT_BUFLEN = 512;
			char readBuffer[DEFAULT_BUFLEN];
			int bufferLength = DEFAULT_BUFLEN;
			ZeroMemory(readBuffer, bufferLength);

			// Read the payload off the network, wait for all the data
			int bytesReceived = Networking::Get()->netReadVRec(clientSocketToGameSever, readBuffer, bufferLength);

			if (bytesReceived > 0)
			{
				// We successfully read some data... 

				ParseReceivedServerPayload(readBuffer);

				RaiseNetworkTrafficReceivedEvent(readBuffer, bytesReceived);

				this->IsDiconnectedFromGameServer = false;
			}
			else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			{
				// Connection closed. Server died.
				this->IsDiconnectedFromGameServer = true;
			}
		}
	}

	void GameClient::ParseReceivedServerPayload(char buffer[512])
	{
		// Parse the payload to find out what kind of message it is (refer to protocol)
		std::string error;
		auto payload = Json::parse(buffer, error);
		auto messageType = payload["messageType"].string_value();		

		if(messageType == "pong")
		{
			/*
			
			Json sendPayload = Json::object
			{
				{ "messageType", "pong" },
				{ "isHappy", true },
				{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
				{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
				{ "ages", Json::array{ 1, 2, 3 } },
				{ "fish", Json::object{ { "yo", "sushi" } } }
			};

			*/
			auto jennyAge = payload["ages"][1].int_value();
			auto bruceAge = payload["ages"][2].int_value();
			const auto& ages = payload["ages"].array_items();
			const auto& fish = payload["fish"].object_items();

			//Do nothing. Dont respond to pong messages...
		}

		if(messageType == "requestPlayerDetails")
		{
			/*
			
			Json sendPayload = Json::object 
			{
				{ "messageType", "requestPlayerDetails" }
			};
			
			*/

			// Send our Nick to the server

			Json payload = Json::object
			{
				{ "messageType", "requestPlayerDetails" },
				{ "Nickname", nickName}
			};

			auto json_str = payload.dump();

			int sendResult = Networking::Get()->netSendVRec(clientSocketToGameSever, json_str.c_str(), json_str.size());
		}

		// The Game server is telling us Someone (Player) has moved 
		if(messageType == ToString(gamelib::EventType::PlayerMovedEventType))
		{
			auto messageType = payload["messageType"].string_value();
			const auto& direction = payload["direction"].string_value();
			const auto& nickname = payload["nickname"].string_value();

			// Send this onto our EventManager but targetting the player with the specified nickname
			// TODO: Deserialize into Event and put onto EventManager
		}
	}

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived)
	{
		auto event = std::shared_ptr<gamelib::NetworkTrafficRecievedEvent>(new NetworkTrafficRecievedEvent(gamelib::EventType::NetworkTrafficReceived, 0));
		event->Message = buffer;
		event->Identifier = "Game Server";
		event->bytesReceived = bytesReceived;

		EventManager::Get()->RaiseEventWithNoLogging(event);
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(std::shared_ptr<Event> evt)
	{
		/* Schedule our events that have occured to be sent to the Game server */
		auto createdEvents = std::vector<std::shared_ptr<Event>>();

		switch(evt->type)
		{
			case gamelib::EventType::PlayerMovedEventType:
				// Our player moved. Tell the game server
				
				auto playerMovedEvent = std::dynamic_pointer_cast<PlayerMovedEvent>(evt);
				
				Json payload = Json::object
				{
					{ "messageType", ToString(evt->type) },
					{ "direction", ToString(playerMovedEvent->direction) },
					{ "nickname", nickName }
				};

				auto json_str = payload.dump();

				int sendResult = Networking::Get()->netSendVRec(clientSocketToGameSever, json_str.c_str(), json_str.size());
				
			break;
		}

		return createdEvents;
	}

	std::string GameClient::GetSubscriberName()
	{
		return "Game Client";
	}

	void GameClient::PingGameServer()
	{
		Json payload = Json::object{
			{ "messageType", "ping" },
			{ "isHappy", false },
			{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		auto json_str = payload.dump();

		int sendResult = Networking::Get()->netSendVRec(clientSocketToGameSever, json_str.c_str(), json_str.size());

		if (sendResult == SOCKET_ERROR) 
		{
			Networking::Get()->netError(0,0, "Ping Game server connect failed. Shutting down client");
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}

	GameClient::~GameClient()
	{
		auto result = shutdown(clientSocketToGameSever, SD_SEND);
		if (result == SOCKET_ERROR) 
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}
}
