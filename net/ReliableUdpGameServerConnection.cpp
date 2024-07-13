#include "ReliableUdpGameServerConnection.h"
#include <net/Networking.h>
#include <events/EventManager.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include <net/PeerInfo.h>
#include <events/Event.h>

namespace gamelib
{
	ReliableUdpGameServerConnection::ReliableUdpGameServerConnection(const std::string& host, const std::string& port)
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

	void ReliableUdpGameServerConnection::Initialize()
	{
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();
		this->eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();
		
	}

	void ReliableUdpGameServerConnection::Listen()
	{
		constexpr auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability					
		
		// Check on udp socket for incoming 'readable' data
		const auto dataIsAvailable = select(maxSockets, &readfds, nullptr, nullptr, &timeout) > 0; 
		
		if (dataIsAvailable)
		{
			CheckForPlayerTraffic();			
		}
	}

	void ReliableUdpGameServerConnection::CheckForPlayerTraffic()
	{
		// Identifier who the client sending the data is
		PeerInfo fromClient; 
				
		// Read all incoming data into readBuffer		
		const int bytesReceived = recvfrom(listeningSocket, receiveBuffer, ReceiveBufferMaxElements, 0,
		                                   reinterpret_cast<sockaddr*>(&fromClient.Address), &fromClient.Length);				

		if (bytesReceived > 0)
		{
			// Hook up to a 32-bit bitfield reader to the received buffer. This will read 32-bits at a time
			const auto count32BitBlocks = bytesReceived * 8 /32;
			BitfieldReader reader(reinterpret_cast<uint32_t*>(receiveBuffer), count32BitBlocks);

			// Where to store the message
			Message message;

			// Unpack receive buffer into message
			message.Read(reader);

			// for the case were received multiple messages (aggregate)
			for( auto& payload : message.Data())
			{
				const char* data = payload.Data();
				ParseReceivedPlayerPayload(data, strlen(data), fromClient);
				RaiseNetworkTrafficReceivedEvent(data, strlen(data), fromClient);
			}
		}
	}

	void ReliableUdpGameServerConnection::RaiseNetworkTrafficReceivedEvent(const char* buffer, const int bytesReceived, const PeerInfo fromClient) 
	{
		std::string identifier = "unknown";
		for(auto player : players)
		{
			if(player.PeerInfo.Address.sin_port == fromClient.Address.sin_port)
			{
				identifier = player.GetNickName();
				break;
			}
		}

		const auto event = eventFactory->CreateNetworkTrafficReceivedEvent(buffer, identifier, bytesReceived,  this->GetSubscriberName());

		eventManager->RaiseEventWithNoLogging(event);
	}

	int ReliableUdpGameServerConnection::InternalSend(const SOCKET socket, const char* buf, int len, const int flags,  const sockaddr* to, int toLen)
	{
		// Get packet to send
		const auto message = reliableUdp.MarkSent(PacketDatum(false, buf));
		
		BitPacker packer(packingBuffer, PackingBufferElements);

		// Pack packet tightly into  buffer before sending
		message->Write(packer); packer.Finish();

		// Count only as much as what was packed
		const auto countBytesToSend = static_cast<int>(ceil(static_cast<double>(packer.TotalBitsPacked()) / static_cast<double>(8)));
		
		// Send over udp
		
		return sendto(socket, reinterpret_cast<char*>(packingBuffer), countBytesToSend, flags, to, toLen);
	}

	void ReliableUdpGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string
	                                                                   & serializedMessage)
	{
		// Loop through all the known players we have and send them this message
		for(auto player : players)
		{
			if(player.GetNickName() == senderNickname)
			{
				continue;
			}

			InternalSend(listeningSocket, serializedMessage.c_str(), static_cast<int>(serializedMessage.length()), 0, (sockaddr*) &player.PeerInfo.Address, player.PeerInfo.Length);
			
		}
	}

	void ReliableUdpGameServerConnection::ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, const PeerInfo fromClient)
	{
		const auto msgHeader = serializationManager->GetMessageHeader(inPayload);
		const auto messageType = msgHeader.MessageType;

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

	void ReliableUdpGameServerConnection::ProcessPingMessage(PeerInfo fromClient)
	{
		const auto data = serializationManager->CreatePongMessage();
		InternalSend(listeningSocket, data.c_str(), static_cast<int>(data.length()), 0, (sockaddr*) &fromClient.Address, fromClient.Length);
	}


	void ReliableUdpGameServerConnection::ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo fromClient)
	{
		bool found = false;
		for(const auto& player : players)
		{
			if(player.PeerInfo.Address.sin_port == fromClient.Address.sin_port)
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

	void ReliableUdpGameServerConnection::SendEventToAllPlayers(const std::string serializedEvent)
	{
		for (auto player : players)
		{
			InternalSend(listeningSocket, serializedEvent.c_str(), static_cast<int>(serializedEvent.size()), 0, (sockaddr*) &player.PeerInfo.Address, player.PeerInfo.Length);
		}
	}

	std::vector<std::shared_ptr<Event>> ReliableUdpGameServerConnection::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		// We dont handle events yet
		return {};
	}

	std::string ReliableUdpGameServerConnection::GetSubscriberName()
	{
		std::stringstream subscriberName;
		subscriberName << host << ":" << port;
		return subscriberName.str();
	}

	void ReliableUdpGameServerConnection::Create()
	{
		listeningSocket = networking->netUdpServer(host.c_str(), port.c_str());
	}

	void ReliableUdpGameServerConnection::Disconnect()
	{		
		closesocket(listeningSocket);
		WSACleanup();
	}
}



