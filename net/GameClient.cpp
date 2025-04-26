#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <events/NetworkTrafficReceivedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include "TransportOnlyProtocolManager.h"
#include "ReliableUdpProtocolManager.h"
#include "events/StartNetworkLevelEvent.h"

using namespace json11;

namespace gamelib
{
	GameClient::GameClient(const std::string& nickName, const std::shared_ptr<IConnectedNetworkSocket>& connection,
	                       const bool useReliableUdpProtocolManager, const bool useEncryption, const Encoding encoding,
	                       const bool sendClientEventsToServer)
	{				
		this->noDataTimeout.tv_sec = 0;
		this->noDataTimeout.tv_usec = 0;
		this->isDisconnectedFromGameServer = true;
		this->eventManager = nullptr;
		this->serializationManager = nullptr;
		this->networking = nullptr;
		this->eventFactory = nullptr;
		this->nickName = nickName;
		this->encoding = encoding;
		this->readfds =
		{
			.fd_count = 0			,
			.fd_array = {0}
		};
		this->sendClientEventsToServer = sendClientEventsToServer;

		SetupProtocolManager(connection, useReliableUdpProtocolManager, useEncryption);
		
		this->isTcp = networkProtocolManager->GetConnection()->IsTcp();
	}

	void GameClient::Initialize()
	{		
		Logger::Get()->LogThis("Initializing game client...");
		
		eventManager = EventManager::Get();
		networking = Networking::Get();
		eventFactory = EventFactory::Get();
		
		serializationManager = std::make_shared<SerializationManager>(encoding);

		networkProtocolManager->Initialize();

		Logger::Get()->LogThis(isTcp 
			? "Client using TCP"
			: "Client using UDP");

		Logger::Get()->LogThis("Nickname:" + std::string(nickName));
		
		SubscribeToGameEvents();
	}

	// Protocol manager that connections to the server are made, and that network events are listened for
	void GameClient::SetupProtocolManager(const std::shared_ptr<IConnectedNetworkSocket>& connection,
	                                      const bool useReliableUdpProtocolManager, bool useEncryption)
	{
		// Layers reliable protocol over transport
		const auto reliableUdpProtocolManager = std::dynamic_pointer_cast<IProtocolManager>(
			std::make_shared<ReliableUdpProtocolManager>(connection, useEncryption));

		// Passes data directly to transports i.e., udp or tcp
		const auto transportOnlyProtocolManager = std::dynamic_pointer_cast<IProtocolManager>(
			std::make_shared<TransportOnlyProtocolManager>(connection));

		this->networkProtocolManager = useReliableUdpProtocolManager
			? reliableUdpProtocolManager
			: transportOnlyProtocolManager;
	}

	void GameClient::SubscribeToGameEvents()
	{
		if (this->sendClientEventsToServer)
		{
			// We subscribe to some of our own events and send them to the game server...
			eventManager->SubscribeToEvent(PlayerMovedEventTypeEventId, this);
			eventManager->SubscribeToEvent(ControllerMoveEventId, this);
		}
	}

	void GameClient::Connect(const std::shared_ptr<GameServer>& inGameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << inGameServer->Address << ":" << inGameServer->Port;
		Logger::Get()->LogThis(message.str());
		
		networkProtocolManager->Connect(inGameServer->Address.c_str(), inGameServer->Port.c_str());
		
		Logger::Get()->LogThis("Server socket created. Connected to server.");
				
		SendPlayerDetails();
						
		if(networkProtocolManager->GetConnection()->IsValid())
		{
			this->isDisconnectedFromGameServer = false;
		}
	}

	void GameClient::SendPlayerDetails() const
	{
		Logger::Get()->LogThis("Registering client with game server.");

		const auto response = serializationManager->CreateRequestPlayerDetailsMessageResponse(nickName);
		
		const int sendResult = networkProtocolManager->Send(response.c_str(), static_cast<int>(response.size()));
		
		Logger::Get()->LogThis(sendResult == 0 ? "Error: 0 bytes sent." : "client/player details successfully sent.");		
	}

	void GameClient::Read(const unsigned long deltaMs)
	{
		if(this->isDisconnectedFromGameServer)
		{
			return;
		}

		constexpr auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment

		// Clear the list of sockets that we are listening for/on
		FD_ZERO(&readfds);
		
		// Add it to the list of file descriptors to listen for readability
		FD_SET(networkProtocolManager->GetConnection()->GetRawSocket(), &readfds);
				
		// Check monitored sockets for incoming 'readable' data
		const auto dataIsAvailable = select(maxSockets, &readfds, nullptr, nullptr, &noDataTimeout) > 0;

		if (dataIsAvailable)
		{
			CheckSocketForTraffic(deltaMs);
		}
	}

	void GameClient::CheckSocketForTraffic(const unsigned long deltaMs)
	{
		// Process network data coming from the game server
		if (FD_ISSET(networkProtocolManager->GetConnection()->GetRawSocket(), &readfds))
		{
			// Read the payload off the network, wait for all the data
			const int bytesReceived = networkProtocolManager->Receive(reinterpret_cast<char*>(readBuffer), ReceiveBufferMaxElements * 32 /8, deltaMs);

			if (bytesReceived > 0)
			{
				ParseReceivedServerPayload(reinterpret_cast<char*>(readBuffer), deltaMs);
				RaiseNetworkTrafficReceivedEvent(reinterpret_cast<char*>(readBuffer), bytesReceived);

				this->isDisconnectedFromGameServer = false;
			}
			else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
			{
				// Connection closed. Server died.
				this->isDisconnectedFromGameServer = true;

				Logger::Get()->LogThis("Connection closed. Server died.");
			}
		}
	}

	void GameClient::ParseReceivedServerPayload(const char* buffer, const unsigned long deltaMs) const
	{
		const auto msgHeader = serializationManager->GetMessageHeader(buffer);
		const auto messageType = msgHeader.MessageType;
		
		// Send client registration response immediately back to server
		if(messageType == "requestPlayerDetails")
		{
			const auto response = serializationManager->CreateRequestPlayerDetailsMessageResponse(nickName);
			
			networkProtocolManager->Send(response.c_str(), static_cast<int>(response.size()), deltaMs);

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

	void GameClient::RaiseNetworkTrafficReceivedEvent(const char* buffer, const int bytesReceived)
	{
		eventManager->RaiseEventWithNoLogging(
			eventFactory->CreateNetworkTrafficReceivedEvent(buffer, "Game Server", bytesReceived,
			                                                this->GetSubscriberName()));
	}

	int GameClient::InternalSend(const std::string& message, const unsigned long deltaMs) const
	{
		return networkProtocolManager->Send(message.c_str(), static_cast<int>(message.size()), deltaMs);
	}

	int GameClient::InternalSend(const char* array, const size_t size, const unsigned long deltaMs) const
	{
		return networkProtocolManager->Send(array, static_cast<int>(size), deltaMs);
	}

	std::vector<std::shared_ptr<Event>> GameClient::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		if (sendClientEventsToServer)
		{
			Logger::Get()->LogThis("Sending client event to server:");
			Logger::Get()->LogThis(evt->ToString());

			const auto message = serializationManager->SerializeEvent(evt, nickName);
			const int sendResult = InternalSend(message, deltaMs);

			Logger::Get()->LogThis(sendResult > 0 
				? "Successfully sent."
				: "Error no data sent");
		}
		return {};
	}

	std::string GameClient::GetSubscriberName()
	{
		return nickName;
	}

	void GameClient::PingGameServer(const unsigned long deltaMs) const
	{
		const auto message = serializationManager->CreatePingMessage();
		const int sendResult = InternalSend(message, deltaMs);

		Logger::Get()->LogThis(sendResult > 0 
			? "Sent ping request"
			: "Ping error, no data sent");

		if (sendResult == SOCKET_ERROR) 
		{
			Networking::netError(0,0, "Ping Game server connect failed. Shutting down client");
			closesocket(networkProtocolManager->GetConnection()->GetRawSocket());
			WSACleanup();
		}
	}

	int GameClient::SendBinary(uint16_t* array, const size_t numBits) const
	{
		const auto numBytes = ceil(static_cast<double>(numBits) / static_cast<double>(8));
		return InternalSend(reinterpret_cast<char*>(array), static_cast<size_t>(numBytes), 0);
	}

	GameClient::~GameClient()
	{
		const auto connectionSocket = networkProtocolManager->GetConnection()->GetRawSocket();

		if (shutdown(connectionSocket, SD_SEND) == SOCKET_ERROR) 
		{
			closesocket(connectionSocket);
			WSACleanup();
		}
	}
}
