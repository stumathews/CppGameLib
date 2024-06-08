#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <events/NetworkTrafficReceivedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include "events/StartNetworkLevelEvent.h"

using namespace json11;

namespace gamelib
{
	GameClient::GameClient(const std::string& nickName, const bool isTcp)
	{
		readBufferLength = 512;				
		noDataTimeout.tv_sec = 0;
		noDataTimeout.tv_usec = 0;	
		clientSocketToGameSever = -1;
		isDisconnectedFromGameServer = true;
		readfds = {0,{0}};
		eventManager = nullptr;
		serializationManager = nullptr;
		networking = nullptr;
		eventFactory = nullptr;
		this->isTcp = isTcp;
		this->nickName = nickName;
	}

	void GameClient::Initialize()
	{		
		Logger::Get()->LogThis("Initializing game client...");
		
		eventManager = EventManager::Get();
		serializationManager = SerializationManager::Get();
		networking = Networking::Get();
		eventFactory = EventFactory::Get();

		Logger::Get()->LogThis(isTcp ? "Client using TCP" : "Client using UDP");
		Logger::Get()->LogThis("Nickname:" + std::string(nickName));

		SubscribeToGameEvents();
	}

	void GameClient::SubscribeToGameEvents()
	{
		// We subscribe to some of our own events and send them to the game server...
		eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
		eventManager->SubscribeToEvent(ControllerMoveEventId, this);
	}

	void GameClient::Connect(const std::shared_ptr<GameServer>& inGameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << inGameServer->Address << ":" << inGameServer->Port;
		Logger::Get()->LogThis(message.str());
		
		clientSocketToGameSever = this->isTcp 
			? networking->netTcpClient(inGameServer->Address.c_str(), inGameServer->Port.c_str())
			: networking->netConnectedUdpClient(inGameServer->Address.c_str(), inGameServer->Port.c_str());
		
		Logger::Get()->LogThis("Server socket created. Connected to server.");
		
		SendPlayerDetails();
						
		if(clientSocketToGameSever)
		{
			this->isDisconnectedFromGameServer = false;
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


	void GameClient::Read()
	{
		if(this->isDisconnectedFromGameServer)
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
			constexpr int maxBufferLength = 512;

			char readBuffer[maxBufferLength];
			constexpr int bufferLength = maxBufferLength;
			ZeroMemory(readBuffer, bufferLength);

			// Read the payload off the network, wait for all the data
			const int bytesReceived = isTcp 
				                          ? Networking::netReadVRec(clientSocketToGameSever, readBuffer, bufferLength)
				                          : recv(clientSocketToGameSever, readBuffer, bufferLength, 0); 

			if (bytesReceived > 0)
			{
				ParseReceivedServerPayload(readBuffer);
				RaiseNetworkTrafficReceivedEvent(readBuffer, bytesReceived);

				this->isDisconnectedFromGameServer = false;
			}
			else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			{
				// Connection closed. Server died.
				this->isDisconnectedFromGameServer = true;
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
				eventManager->DispatchEventToSubscriber(event, 0UL);
				return;
			}

			// Notice target, send only to a specific target
			eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);	
		}
	}

	void GameClient::RaiseNetworkTrafficReceivedEvent(char  buffer[512], const int bytesReceived)
	{
		eventManager->RaiseEventWithNoLogging(eventFactory->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived, this->GetSubscriberName()));
	}

	int GameClient::InternalSend(const std::string& message) const
	{
		return isTcp
			       ? Networking::netSendVRec(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()))
			       : send(clientSocketToGameSever, message.c_str(), static_cast<int>(message.size()), 0);
	}

	int GameClient::InternalSend(const char* array, const size_t size) const
	{
		return isTcp
			       ? Networking::netSendVRec(clientSocketToGameSever, array, static_cast<int>(size))
			       : send(clientSocketToGameSever, array, static_cast<int>(size), 0);
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{		
		std::vector<std::shared_ptr<Event>> createdEvents;

		Logger::Get()->LogThis("Sending client event to server:");
		Logger::Get()->LogThis(evt->ToString());

		const auto message = serializationManager->Serialize(evt, nickName);
		const int sendResult = InternalSend(message);

		Logger::Get()->LogThis(sendResult > 0 ? "Successfully sent." : "Error no data sent");

		return createdEvents;
	}

	std::string GameClient::GetSubscriberName()
	{
		return nickName;
	}

	void GameClient::PingGameServer() const
	{
		Logger::Get()->LogThis("Pinging game server...");

		const auto message = serializationManager->CreatePingMessage();
		const int sendResult = InternalSend(message);

		Logger::Get()->LogThis(sendResult > 0 ? "Sent ping request" : "Error, no data sent");

		if (sendResult == SOCKET_ERROR) 
		{
			Networking::netError(0,0, "Ping Game server connect failed. Shutting down client");
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}

	int GameClient::SendBinary(uint16_t* array, const size_t numBits) const
	{
		const auto numBytes = ceil((double)numBits / (double)8);
		return InternalSend(reinterpret_cast<char*>(array), numBytes);
	}

	GameClient::~GameClient()
	{
		if (shutdown(clientSocketToGameSever, SD_SEND) == SOCKET_ERROR) 
		{
			closesocket(clientSocketToGameSever);
			WSACleanup();
		}
	}
}
