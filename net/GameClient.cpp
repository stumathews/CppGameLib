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
		this->networking = nullptr;
		this->eventFactory = nullptr;
	}

	void GameClient::Initialize()
	{
		this->nickName  = SettingsManager::Get()->GetString("networking", "nickname");
		this->eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();

		// We subscribe to some of our own events and send them to the game server...
		eventManager->SubscribeToEvent(EventType::PlayerMovedEventType, this);
		eventManager->SubscribeToEvent(EventType::ControllerMoveEvent, this);
		eventManager->SubscribeToEvent(EventType::Fire, this);
	}

	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;
		Logger::Get()->LogThis(message.str());
		
		clientSocketToGameSever = networking->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str());			
						
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
			int bytesReceived = networking->netReadVRec(clientSocketToGameSever, readBuffer, bufferLength);

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
		auto msgHeader = serializationManager->GetMessageHeader(buffer);
		auto messageType = msgHeader.MessageType;

		// We recieved some data from the game server:
		
		if(messageType == "requestPlayerDetails")
		{
			auto response = serializationManager->CreateRequestPlayerDetailsMessageResponse(nickName);

			int sendResult = networking->netSendVRec(clientSocketToGameSever, response.c_str(), response.size());
		}
		else 
		{
			// These are player messages arriving from the game server, pent them to their targets
			auto event = serializationManager->Deserialize(msgHeader, buffer);
			if(event)
			{
				eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);	
			}
			
		}
	}

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived)
	{
		eventManager->RaiseEventWithNoLogging(eventFactory->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived));
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(std::shared_ptr<Event> evt)
	{		
		auto message = serializationManager->Serialize(evt, nickName);
		int sendResult = networking->netSendVRec(clientSocketToGameSever, message.c_str(), message.size());

		return std::vector<std::shared_ptr<Event>>();;
	}

	std::string GameClient::GetSubscriberName()
	{
		return "Game Client";
	}

	void GameClient::PingGameServer()
	{
		auto message = serializationManager->CreatePingMessage();
		int sendResult = networking->netSendVRec(clientSocketToGameSever, message.c_str(), message.size());

		if (sendResult == SOCKET_ERROR) 
		{
			networking->netError(0,0, "Ping Game server connect failed. Shutting down client");
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}

	GameClient::~GameClient()
	{
		if (shutdown(clientSocketToGameSever, SD_SEND) == SOCKET_ERROR) 
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}
}
