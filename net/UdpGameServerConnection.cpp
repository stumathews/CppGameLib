#include "UdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>
#include <net/PeerInfo.h>
#include <events/NetworkPlayerJoinedEvent.h>
#include <events/Event.h>

namespace gamelib
{
	UdpGameServerConnection::UdpGameServerConnection(std::string host, std::string port)
	{
		this->host = host;
		this->port = port;
		this->timeout.tv_sec = 0;
		this->timeout.tv_usec = 0;
		this->serializationManager = nullptr;
		this->eventManager = nullptr;
		this->networking = nullptr;
		this->eventFactory = nullptr;
	}

	void UdpGameServerConnection::Initialize()
	{
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();
		this->eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();
		
	}

	void UdpGameServerConnection::Listen()
	{
		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability					
		
		// Check on udp socket for incoming 'readable' data
		auto dataIsAvailable = select(maxSockets, &readfds, NULL, NULL, &timeout) > 0; 
		
		if (dataIsAvailable)
		{
			CheckForPlayerTraffic();			
		}
	}

	void UdpGameServerConnection::CheckForPlayerTraffic()
	{
		const int DEFAULT_BUFLEN = 512;
		char buffer[DEFAULT_BUFLEN];
		int bufferLength = DEFAULT_BUFLEN;
		ZeroMemory(buffer, bufferLength);
				
		PeerInfo fromClient; // Identifier who the client sending the data is

		// Read all incoming data		
		int bytesReceived = recvfrom(listeningSocket, buffer, bufferLength, 0, (sockaddr*) &fromClient.Address, &fromClient.Length);

		if (bytesReceived > 0)
		{
			ParseReceivedPlayerPayload(buffer, bufferLength, fromClient);
			RaiseNetworkTrafficReceievedEvent(buffer, bytesReceived, fromClient);
		}
	}

	void UdpGameServerConnection::RaiseNetworkTrafficReceievedEvent(char buffer[512], int bytesReceived, PeerInfo fromClient)
	{
		std::string identifier = "unknown";
		for(auto player : Players)
		{
			if(player.peerInfo.Address.sin_port == fromClient.Address.sin_port)
			{
				identifier = player.GetNickName();
				break;
			}
		}

		auto event = eventFactory->CreateNetworkTrafficReceivedEvent(buffer, identifier, bytesReceived);

		eventManager->RaiseEventWithNoLogging(event);
	}

	void UdpGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage)
	{
		// Loop through all the known players we have and send them this message
		for(auto player : Players)
		{
			if(player.GetNickName() == senderNickname)
			{
				continue;
			}

			sendto(listeningSocket, serializedMessage.c_str(), serializedMessage.length(), 0, (sockaddr*) &player.peerInfo.Address, player.peerInfo.Length);
		}
	}

	void UdpGameServerConnection::ParseReceivedPlayerPayload(char* inPayload, int payloadLength, PeerInfo fromClient)
	{
		auto msgHeader = serializationManager->GetMessageHeader(inPayload);
		auto messageType = msgHeader.MessageType;

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
			if(auto event = serializationManager->Deserialize(msgHeader, inPayload))
			{
				eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);
			}
		}
	}

	void UdpGameServerConnection::ProcessPingMessage(PeerInfo fromClient)
	{
		auto data = serializationManager->CreatePongMessage();
		sendto(listeningSocket, data.c_str(), data.length(), 0, (sockaddr*) &fromClient.Address, fromClient.Length);
	}


	void UdpGameServerConnection::ProcessRequestPlayerDetailsMessage(gamelib::MessageHeader& messageHeader, PeerInfo fromClient)
	{
		bool found = false;
		for(auto player : Players)
		{
			if(player.peerInfo.Address.sin_port == fromClient.Address.sin_port)
			{
				found = true;
			}
		}

		// Add new player
		if(!found)
		{
			auto newPlayer = UdpNetworkPlayer(fromClient, messageHeader.MessageTarget);
			Players.push_back(newPlayer);
			
			eventManager->RaiseEvent(eventFactory->CreateNetworkPlayerJoinedEvent(newPlayer), this);
		}
	}

	void UdpGameServerConnection::SendEventToAllPlayers(std::string serializedEvent)
	{
		for (auto player : Players)
		{
			int sendResult = sendto(listeningSocket, serializedEvent.c_str(), serializedEvent.size(), 0, (sockaddr*) &player.peerInfo.Address, player.peerInfo.Length);
		}
	}

	std::vector<std::shared_ptr<Event>> UdpGameServerConnection::HandleEvent(std::shared_ptr<Event> evt)
	{
		// We dont handle events yet
		return std::vector<std::shared_ptr<Event>>();
	}

	std::string UdpGameServerConnection::GetSubscriberName()
	{
		return "UdpGameServerConnection";
	}

	void UdpGameServerConnection::Create()
	{
		listeningSocket = networking->netUdpServer(host.c_str(), port.c_str());
	}

}