#include "UdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include <net/PeerInfo.h>
#include <events/Event.h>
#include <events/NetworkTrafficReceivedEvent.h>

namespace gamelib
{
	UdpGameServerConnection::UdpGameServerConnection(const std::string& host, const std::string& port, const gamelib::Encoding wireFormat)
	{
		this->host = host;
		this->port = port;
		this->timeout.tv_sec = 0;
		this->timeout.tv_usec = 0;
		this->serializationManager = nullptr;
		this->eventManager = nullptr;
		this->networking = nullptr;
		this->eventFactory = nullptr;
		this->TheEncoding = wireFormat;
	}

	void UdpGameServerConnection::Initialize()
	{
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();
		this->eventManager = EventManager::Get();
		this->serializationManager = std::make_shared<SerializationManager>(TheEncoding);
		
	}	

	void UdpGameServerConnection::Create()
	{
		listeningSocket = networking->netUdpServer(host.c_str(), port.c_str());
	}

	void UdpGameServerConnection::Disconnect()
	{		
		NETCLOSE(listeningSocket);
		EXIT(listeningSocket);
	}

	void UdpGameServerConnection::Listen(const unsigned long deltaMs)
	{
		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability					
		
		// Check on udp socket for incoming 'readable' data
		const auto dataIsAvailable = select(maxSockets, &readfds, nullptr, nullptr, &timeout) > 0; 
		
		if (dataIsAvailable)
		{
			CheckForPlayerTraffic(deltaMs);			
		}
	}

	void UdpGameServerConnection::CheckForPlayerTraffic(unsigned long deltaMs)
	{
				
		PeerInfo fromClient; // Identifier who the client sending the data is

		// Read all incoming data		
		const int bytesReceived = recvfrom(listeningSocket, reinterpret_cast<char*>(readBuffer), ReadBufferSizeInBytes,
		                                   0, reinterpret_cast<sockaddr*>(&fromClient.Address), &fromClient.Length);

		if (bytesReceived > 0)
		{
			ParseReceivedPlayerPayload(reinterpret_cast<char*>(readBuffer), ReadBufferSizeInBytes, fromClient);
			RaiseNetworkTrafficReceivedEvent(reinterpret_cast<char*>(readBuffer), bytesReceived, fromClient);
		}
	}

	void UdpGameServerConnection::RaiseNetworkTrafficReceivedEvent(const char* buffer, const int bytesReceived,
	                                                               const PeerInfo fromClient) 
	{
		std::string identifier = "unknown";
		for(auto player : players)
		{
			if(player.ThePeerInfo.Address.sin_port == fromClient.Address.sin_port)
			{
				identifier = player.GetNickName();
				break;
			}
		}

		const auto event = eventFactory->CreateNetworkTrafficReceivedEvent(buffer, identifier, bytesReceived,  this->GetSubscriberName());

		eventManager->RaiseEventWithNoLogging(event);
	}

	int UdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, const int len, const int flags,
	                                          const sockaddr* to, const int toLen, unsigned long sendTimeMs)
	{		
		return sendto(socket, buf, len, flags, to, toLen);
	}

	void UdpGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string
	                                                                   & serializedMessage)
	{
		// Loop through all the known players we have and send them this message
		for(auto player : players)
		{
			if(player.GetNickName() == senderNickname)
			{
				continue;
			}

			InternalSend(listeningSocket, serializedMessage.c_str(), static_cast<int>(serializedMessage.length()), 0, (sockaddr*) &player.ThePeerInfo.Address, player.ThePeerInfo.Length);
			
		}
	}

	void UdpGameServerConnection::ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, const PeerInfo fromClient)
	{
		const auto msgHeader = serializationManager->GetMessageHeader(inPayload);
		const auto messageType = msgHeader.TheMessageType;

		if(messageType == "ping")
		{	
			ProcessPingMessage(fromClient);
		}
		else if(messageType == "requestPlayerDetails")
		{
			ProcessRequestPlayerDetailsMessage(msgHeader, fromClient);
		}		
		else
		{
			// Send to other network players
			SendToConnectedPlayersExceptToSender(msgHeader.MessageTarget, inPayload);

			// Send to ourself
			if(const auto event = serializationManager->Deserialize(msgHeader, inPayload))
			{
				eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);
			}
		}
	}

	void UdpGameServerConnection::ProcessPingMessage(PeerInfo fromClient)
	{
		const auto data = serializationManager->CreatePongMessage();
		InternalSend(listeningSocket, data.c_str(), static_cast<int>(data.length()), 0, reinterpret_cast<sockaddr*>(&fromClient.Address), fromClient.Length);
	}


	void UdpGameServerConnection::ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo fromClient)
	{
		bool found = false;
		for(const auto& player : players)
		{
			if(player.ThePeerInfo.Address.sin_port == fromClient.Address.sin_port)
			{
				found = true;
			}
		}

		// Add new player
		if(!found)
		{
			const auto newPlayer = UdpNetworkPlayer(fromClient, messageHeader.MessageTarget);
			players.push_back(newPlayer);
			
			eventManager->RaiseEvent(eventFactory->CreateNetworkPlayerJoinedEvent(newPlayer), this);
		}
	}

	void UdpGameServerConnection::SendEventToAllPlayers(const std::string serializedEvent)
	{
		for (auto player : players)
		{
			InternalSend(listeningSocket, serializedEvent.c_str(), static_cast<int>(serializedEvent.size()), 0, reinterpret_cast<sockaddr*>(&player.ThePeerInfo.Address), player.ThePeerInfo.Length);
		}
	}

	std::vector<std::shared_ptr<Event>> UdpGameServerConnection::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		// We don't handle events yet
		return {};
	}

	std::string UdpGameServerConnection::GetSubscriberName()
	{
		std::stringstream subscriberName;
		subscriberName << host << ":" << port;
		return subscriberName.str();
	}
}
