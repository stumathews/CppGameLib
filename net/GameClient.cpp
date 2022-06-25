#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>
#include <events/PlayerMovedEvent.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>

using namespace json11;

namespace gamelib
{
	GameClient::GameClient()
	{
		this->readBufferLength = 512;				
		this->noDataTimeout.tv_sec = 0;
		this->noDataTimeout.tv_usec = 0;	
		this->clientSocketToGameSever = -1;
		this->IsDiconnectedFromGameServer = true;
		this->readfds = {0};
		this->eventManager = nullptr;
		this->serializationManager = nullptr;
	}

	void GameClient::Initialize()
	{
		this->nickName  = SettingsManager::Get()->GetString("networking", "nickname");
		this->eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();

		// We subscribe to some of our own events and send them to the game server...
		EventManager::Get()->SubscribeToEvent(gamelib::EventType::PlayerMovedEventType, this);
	}

	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;
		Logger::Get()->LogThis(message.str());
		
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
			CheckSocketForTraffic();
		}
	}

	void GameClient::CheckSocketForTraffic()
	{
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

				// Automatically log all data received by the client from the server
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
		auto msgHeader = SerializationManager::Get()->GetMessageHeader(buffer);
		auto messageType = msgHeader.MessageType;

		// We recieved some data from the game server:
		
		if(messageType == "requestPlayerDetails")
		{
			auto response = SerializationManager::Get()->CreateRequestPlayerDetailsMessageResponse(nickName);

			int sendResult = Networking::Get()->netSendVRec(clientSocketToGameSever, response.c_str(), response.size());
		}
		else 
		{
			// These are player messages arriving from the game server, pent them to their targets
			auto event = serializationManager->DeserializeToEvent(msgHeader, buffer);
			if(event)
			{
				eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);	
			}
			
		}
	}

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived)
	{		
		auto event = EventFactory::Get()->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived);
		EventManager::Get()->RaiseEventWithNoLogging(event);
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(std::shared_ptr<Event> evt)
	{
		/* Schedule our events that have occured to be sent to the Game server */
		auto createdEvents = std::vector<std::shared_ptr<Event>>();

		auto message = serializationManager->CreateEventMessage(evt, nickName);

		// Send message to game server
		int sendResult = Networking::Get()->netSendVRec(clientSocketToGameSever, message.c_str(), message.size());

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
