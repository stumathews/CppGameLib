#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <events/NetworkTrafficReceivedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>

#include "events/StartNetworkLevelEvent.h"
#include "util/SettingsManager.h"

using namespace json11;

namespace gamelib
{
	GameClient::GameClient()
	{
		this->readBufferLength = 512;				
		this->noDataTimeout.tv_sec = 0;
		this->noDataTimeout.tv_usec = 0;	
		this->clientSocketToGameSever = -1;
		this->IsDisconnectedFromGameServer = true;
		this->readfds = {0,{0}};
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
		_eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		_eventManager->SubscribeToEvent(ControllerMoveEventId, this);
	}

	void GameClient::Connect(const std::shared_ptr<GameServer>& inGameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << inGameServer->address << ":" << inGameServer->port;
		Logger::Get()->LogThis(message.str());
		
		clientSocketToGameSever = this->isTcp 
			? networking->netTcpClient(inGameServer->address.c_str(), inGameServer->port.c_str())
			: networking->netConnectedUdpClient(inGameServer->address.c_str(), inGameServer->port.c_str());
		
		Logger::Get()->LogThis("Server socket created. Connected to server.");
		
		SendPlayerDetails();
						
		if(clientSocketToGameSever)
		{
			this->IsDisconnectedFromGameServer = false;
		}
	}
	void GameClient::SendPlayerDetails() const
	{
		Logger::Get()->LogThis("Registering client with game server.");

		const auto response = SerializationManager::CreateRequestPlayerDetailsMessageResponse(nickName);

		const int sendResult = isTcp
			                       ? Networking::netSendVRec(clientSocketToGameSever, response.c_str(), static_cast<int>(response.size()))
			                       : send(clientSocketToGameSever, response.c_str(), static_cast<int>(response.size()), 0);
		
		Logger::Get()->LogThis(sendResult == 0 ? "Error: 0 bytes sent." : "client/player details successfully sent.");
		
	}


	void GameClient::Listen()
	{
		if(this->IsDisconnectedFromGameServer)
		{
			return;
		}

		constexpr auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment

		// Clear the list of sockets that we are listening for/on
		FD_ZERO(&readfds);
		
		// Add it to the list of file descriptors to listen for readability
		FD_SET(clientSocketToGameSever, &readfds);
				
		// Check monitored sockets for incoming 'readable' data
		const auto dataIsAvailable = select(maxSockets, &readfds, nullptr, nullptr, &noDataTimeout) > 0;

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
			const int bufferLength = DEFAULT_BUFLEN;
			ZeroMemory(readBuffer, bufferLength);

			// Read the payload off the network, wait for all the data
			const int bytesReceived = isTcp 
				                          ? Networking::netReadVRec(clientSocketToGameSever, readBuffer, bufferLength)
				                          : recv(clientSocketToGameSever, readBuffer, bufferLength, 0); 

			if (bytesReceived > 0)
			{
				ParseReceivedServerPayload(readBuffer);
				RaiseNetworkTrafficReceivedEvent(readBuffer, bytesReceived);

				this->IsDisconnectedFromGameServer = false;
			}
			else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			{
				// Connection closed. Server died.
				this->IsDisconnectedFromGameServer = true;
			}
		}
	}

	void GameClient::ParseReceivedServerPayload(char buffer[512]) const
	{
		const auto msgHeader = serializationManager->GetMessageHeader(buffer);
		const auto messageType = msgHeader.MessageType;
		
		// Send client registration response immediately back to server
		if(messageType == "requestPlayerDetails")
		{
			const auto response = SerializationManager::CreateRequestPlayerDetailsMessageResponse(nickName);

			isTcp ? Networking::netSendVRec(clientSocketToGameSever, response.c_str(), static_cast<int>(static_cast<int>(response.size())))
				: send(clientSocketToGameSever, response.c_str(), static_cast<int>(static_cast<int>(response.size())), 0);
			return;
		}
		
		// This puts events sent by the game server, over the network, onto our local event stack.
		const auto event = serializationManager->Deserialize(msgHeader, buffer);
		
		// Some events are known not to have a specific subscriber target
		const bool noTarget = messageType == StartNetworkLevelEventId.Name;

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

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], const int bytesReceived) const
	{
		_eventManager->RaiseEventWithNoLogging(_eventFactory->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived));
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(const std::shared_ptr<Event> evt, unsigned long deltaMs)
	{		
		std::vector<std::shared_ptr<Event>> createdEvents;

		Logger::Get()->LogThis("Sending client event to server:");
		Logger::Get()->LogThis(evt->ToString());

		const auto message = serializationManager->Serialize(evt, nickName);
		const int sendResult = isTcp ? Networking::netSendVRec(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()))
			                       : send(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()), 0);

		Logger::Get()->LogThis(sendResult > 0 ? "Successfully sent." : "Error no data sent");

		return createdEvents;
	}

	std::string GameClient::GetSubscriberName()
	{
		return "Game Client";
	}

	void GameClient::PingGameServer() const
	{
		Logger::Get()->LogThis("Pinging game server...");

		const auto message = serializationManager->CreatePingMessage();
		const int sendResult = isTcp ? Networking::netSendVRec(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()))
			                       : send(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()), 0);

		Logger::Get()->LogThis(sendResult > 0 ? "Sent ping request" : "Error, no data sent");

		if (sendResult == SOCKET_ERROR) 
		{
			Networking::netError(0,0, "Ping Game server connect failed. Shutting down client");
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
