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
		this->_eventManager = nullptr;
		this->serializationManager = nullptr;
		this->networking = nullptr;
		this->_eventFactory = nullptr;
		this->isTcp = true;
	}

	void GameClient::Initialize()
	{		
		Logger::Get()->LogThis("Initializing game client...");

		this->_eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();
		this->networking = Networking::Get();
		this->_eventFactory = EventFactory::Get();

		this->nickName  = SettingsManager::Get()->GetString("networking", "nickname");
		this->isTcp = SettingsManager::Get()->GetBool("networking", "isTcp");

		Logger::Get()->LogThis(isTcp ? "Client using TCP" : "Client using UDP");
		Logger::Get()->LogThis("Nickname:");
		Logger::Get()->LogThis(nickName);

		SubscribeToGameEvents();
	}

	void GameClient::SubscribeToGameEvents()
	{
		// We subscribe to some of our own events and send them to the game server...
		_eventManager->SubscribeToEvent(EventType::PlayerMovedEventType, this);
		_eventManager->SubscribeToEvent(EventType::ControllerMoveEvent, this);
		_eventManager->SubscribeToEvent(EventType::Fire, this);
	}

	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;
		Logger::Get()->LogThis(message.str());
		
		clientSocketToGameSever = this->isTcp 
			? networking->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str())
			: networking->netConnectedUdpClient(gameServer->address.c_str(), gameServer->port.c_str());
		
		Logger::Get()->LogThis("Server socket created. Connected to server.");
		
		SendPlayerDetails();
						
		if(clientSocketToGameSever)
		{
			this->IsDiconnectedFromGameServer = false;
		}
	}
	void GameClient::SendPlayerDetails()
	{
		Logger::Get()->LogThis("Regestering client with game server.");

		auto response = serializationManager->CreateRequestPlayerDetailsMessageResponse(nickName);

		int sendResult = isTcp
			? networking->netSendVRec(clientSocketToGameSever, response.c_str(), response.size())
			: send(clientSocketToGameSever, response.c_str(), response.size(), 0);
		
		Logger::Get()->LogThis(sendResult == 0 ? "Error: 0 bytes sent." : "client/player details successfully sent.");
		
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
		// Process network data coming from the game server
		if (FD_ISSET(clientSocketToGameSever, &readfds))
		{
			const int DEFAULT_BUFLEN = 512;
			char readBuffer[DEFAULT_BUFLEN];
			int bufferLength = DEFAULT_BUFLEN;
			ZeroMemory(readBuffer, bufferLength);

			// Read the payload off the network, wait for all the data
			int bytesReceived = isTcp 
					? networking->netReadVRec(clientSocketToGameSever, readBuffer, bufferLength)
					: recv(clientSocketToGameSever, readBuffer, bufferLength, 0); 

			if (bytesReceived > 0)
			{
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
		auto msgHeader = serializationManager->GetMessageHeader(buffer);
		auto messageType = msgHeader.MessageType;
		
		// Send client registration response immediately back to server
		if(messageType == "requestPlayerDetails")
		{
			auto response = serializationManager->CreateRequestPlayerDetailsMessageResponse(nickName);

			int sendResult = isTcp ? networking->netSendVRec(clientSocketToGameSever, response.c_str(), response.size())
								   : send(clientSocketToGameSever, response.c_str(), response.size(), 0);
			return;
		}
		
		// This puts events sent by the game server, over the network, onto our local event stack.
		auto event = serializationManager->Deserialize(msgHeader, buffer);
		
		// Some events are known not to have a specific subscriber target
		bool noTarget = messageType == ToString(EventType::StartNetworkLevel);

		if(event)
		{			
			if (noTarget)
			{
				_eventManager->DispatchEventToSubscriber(event, 0UL);
				return;
			}

			// Notice target, send only to a specific target
			_eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);	
		}
	}

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], int bytesReceived)
	{
		_eventManager->RaiseEventWithNoLogging(_eventFactory->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived));
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(std::shared_ptr<Event> evt, unsigned long deltaMs)
	{		
		std::vector<std::shared_ptr<Event>> createdEvents;

		Logger::Get()->LogThis("Sending client event to server:");
		Logger::Get()->LogThis(evt->ToString());

		auto message = serializationManager->Serialize(evt, nickName);
		int sendResult = isTcp ? networking->netSendVRec(clientSocketToGameSever, message.c_str(), message.size())
							   : send(clientSocketToGameSever, message.c_str(), message.size(), 0);

		Logger::Get()->LogThis(sendResult > 0 ? "Successfully sent." : "Error no data sent");

		return createdEvents;
	}

	std::string GameClient::GetSubscriberName()
	{
		return "Game Client";
	}

	void GameClient::PingGameServer()
	{
		Logger::Get()->LogThis("Pinging game server...");

		auto message = serializationManager->CreatePingMessage();
		int sendResult = isTcp ? networking->netSendVRec(clientSocketToGameSever, message.c_str(), message.size())
							   : send(clientSocketToGameSever, message.c_str(), message.size(), 0);

		Logger::Get()->LogThis(sendResult > 0 ? "Sent ping request" : "Error, no data sent");

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
