#include "TcpGameServerConnection.h"
#include <net/Networking.h>
#include <net/PeerInfo.h>
#include <events/EventManager.h>
#include <events/EventFactory.h>
#include <file/SerializationManager.h>
#include <events/NetworkTrafficReceivedEvent.h>

#include "file/Logger.h"

namespace gamelib
{
	TcpGameServerConnection::TcpGameServerConnection(const std::string& host, const std::string& port, const gamelib::Encoding wireFormat)
	{
		this->host = host;
		this->port = port;
		this->timeout.tv_sec = 0;
		this->timeout.tv_usec = 0;
		this->serializationManager = nullptr;
		this->_eventManager = nullptr;
		this->networking = nullptr;
		this->_eventFactory = nullptr;
		this->encoding = wireFormat;
	}
	
	void TcpGameServerConnection::Initialize()
	{
		this->networking = Networking::Get();
		this->_eventFactory = EventFactory::Get();
		this->_eventManager = EventManager::Get();
		this->serializationManager = std::make_shared<SerializationManager>(encoding);		
	}

	void TcpGameServerConnection::Listen(const unsigned long deltaMs)
	{
		constexpr auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability
				
		
		// Also listen for any traffic on any of the player's sockets
		for(const auto& player : Players)
		{
			if(player.IsConnected())
			{
				FD_SET(player.GetSocket(), &readfds);
			}
		}
		
		
		// Check monitored sockets for incoming 'readable' data
		const auto dataIsAvailable = select(maxSockets, &readfds, nullptr, nullptr, &timeout) > 0; 
		
		if (dataIsAvailable)
		{			
			CheckForNewTcpPlayers();
			CheckForPlayerTraffic(deltaMs);			
		}
	}

	void TcpGameServerConnection::CheckForNewTcpPlayers()
	{
		if (FD_ISSET(listeningSocket, &readfds))
		{
			PeerInfo peerInfo;
			peerInfo.Length = sizeof(peerInfo.Address);
			const SOCKET connectedSocket = accept(listeningSocket, reinterpret_cast<sockaddr *>(&peerInfo.Address), &peerInfo.Length);

			if (connectedSocket != INVALID_SOCKET_HANDLE)
			{
				const auto newPlayer = TcpNetworkPlayer(connectedSocket);
				Players.push_back(newPlayer); // Store incoming player sockets so we can listen for incoming player data too
				
				//eventManager->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
				_eventManager->RaiseEvent(_eventFactory->CreateNetworkPlayerJoinedEvent(newPlayer), this);
			}			
			
		}
	}

	std::vector<std::shared_ptr<Event>> TcpGameServerConnection::HandleEvent(const std::shared_ptr<Event>& evt, const unsigned long deltaMs)
	{
		return {};
	}

	std::string TcpGameServerConnection::GetSubscriberName()
	{
		std::stringstream subscriberName;
		subscriberName << host << ":" << port;
		return subscriberName.str();
	}

	void TcpGameServerConnection::Create()
	{
		listeningSocket = networking->netTcpServer(host.c_str(), port.c_str());
	}

	void TcpGameServerConnection::Disconnect()
	{
		for (const auto & player : Players)
		{
			// disconnect client socket
			const auto clientSocket = player.GetSocket();

			// Let the client know we're shutting them down...
			const auto iResult = shutdown(clientSocket, SHUT_SEND);
			if (iResult == SOCKET_ERROR) 
			{
				// maybe the client is already finished. Close Socket.
				NETCLOSE(clientSocket);
			}
		}

		// disconnect listening socket so its no longer binding to the listening port
		CLOSE(listeningSocket);		
	}

	void TcpGameServerConnection::CheckForPlayerTraffic(unsigned long deltaMs)
	{
		for (size_t playerId = 0; playerId < Players.size(); playerId++)
		{
			if(!Players[playerId].IsConnected())
			{
				continue;
			}

			if (FD_ISSET(Players[playerId].GetSocket(), &readfds))
			{
				constexpr int DEFAULT_BUFLEN = 512;
				char buffer[DEFAULT_BUFLEN];
				constexpr int bufferLength = DEFAULT_BUFLEN;
				ZeroMemory(buffer, bufferLength);

				// Read all incoming data
				const int bytesReceived = networking->netReadVRec(Players[playerId].GetSocket(), buffer, bufferLength);

				if (bytesReceived > 0)
				{
					ParseReceivedPlayerPayload(playerId, buffer, bufferLength);
					RaiseNetworkTrafficReceivedEvent(buffer, playerId, bytesReceived);
				}

				if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
				{
					// Connection closed. Remove the socket from those being monitored for incoming traffic
					Players[playerId].SetIsConnected(false);

					const auto result = shutdown(Players[playerId].GetSocket(), SD_BOTH);
					if (result == SOCKET_ERROR)
					{
						printf("shutdown failed: %d\n", WSAGetLastError());
						NETCLOSE(Players[playerId].GetSocket());
					}
				}
			}
		}
	}

	void TcpGameServerConnection::ParseReceivedPlayerPayload(const size_t& playerId, const char* inPayload, int payloadLength)
	{
		if(!Players[playerId].IsConnected())
		{
			return;
		}

		const auto msgHeader = serializationManager->GetMessageHeader(inPayload);
		const auto messageType = msgHeader.TheMessageType;

		if(messageType == "ping")
		{	
			ProcessPingMessage(playerId);
		}
		else if(messageType == "requestPlayerDetails")
		{
			ProcessRequestPlayerDetailsMessage(static_cast<int>(playerId), msgHeader);
		}		
		else
		{
			// Send to other network players
			SendToConnectedPlayersExceptToSender(msgHeader.MessageTarget, inPayload, playerId);

			// Send to ourself
			const auto event = serializationManager->Deserialize(msgHeader, inPayload);
			if(event)
			{
				_eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);
			}
		}

	}

	void TcpGameServerConnection::RaiseNetworkTrafficReceivedEvent(char buffer[512], const size_t& i, const int bytesReceived) 
	{
		const auto event = _eventFactory->CreateNetworkTrafficReceivedEvent(buffer, std::to_string(i + 1), bytesReceived, GetSubscriberName());

		_eventManager->RaiseEventWithNoLogging(event);
	}

	void TcpGameServerConnection::ProcessPingMessage(const size_t& playerId) const
	{
		const auto data = serializationManager->CreatePongMessage();
		networking->netSendVRec(Players[playerId].GetSocket(), data.c_str(), static_cast<int>(data.length()));
	}

	void TcpGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string
	                                                                   & serializedMessage, const size_t& playerId) const
	{
		for (auto player : Players)
		{
			if (!player.IsConnected() || player.GetNickName() == senderNickname)
			{
				continue;
			}

			networking->netSendVRec(Players[playerId].GetSocket(), serializedMessage.c_str(), static_cast<int>(serializedMessage.length()));
		}
	}

	void TcpGameServerConnection::ProcessRequestPlayerDetailsMessage(const int playerId, const MessageHeader& messageHeader)
	{
		// Record the players nickname
		Players[playerId].SetNickName(messageHeader.MessageTarget);
	}

	void TcpGameServerConnection::SendEventToAllPlayers(const std::string serializedEvent)
	{		
		for (const auto& player : Players)
		{
			if (!player.IsConnected())
			{
				continue;
			}

			networking->netSendVRec(player.GetSocket(), serializedEvent.c_str(), static_cast<int>(serializedEvent.size()));
		}
	}	
	
	TcpGameServerConnection::~TcpGameServerConnection()
	{
		for(const auto& player : Players)
		{			
			CLOSE(player.GetSocket());
			Logger::Get()->LogThis("Closed player connection.");
		}
	}
}
