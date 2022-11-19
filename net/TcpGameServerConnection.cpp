#include "TcpGameServerConnection.h"
#include <net/Networking.h>
#include <net/PeerInfo.h>
#include <events/EventManager.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>
#include <events/NetworkPlayerJoinedEvent.h>

namespace gamelib
{
	TcpGameServerConnection::TcpGameServerConnection(std::string host, std::string port)
	{
		this->host = host;
		this->port = port;
		this->timeout.tv_sec = 0;
		this->timeout.tv_usec = 0;
		this->serializationManager = nullptr;
		this->_eventManager = nullptr;
		this->networking = nullptr;
		this->_eventFactory = nullptr;
	}
	
	void TcpGameServerConnection::Initialize()
	{
		this->networking = Networking::Get();
		this->_eventFactory = EventFactory::Get();
		this->_eventManager = EventManager::Get();
		this->serializationManager = SerializationManager::Get();		
	}

	void TcpGameServerConnection::Listen()
	{
		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability
				
		
		// Also listen for any traffic on any of the player's sockets
		for(auto player : Players)
		{
			if(player.IsConnected())
			{
				FD_SET(player.GetSocket(), &readfds);
			}
		}
		
		
		// Check monitored sockets for incoming 'readable' data
		auto dataIsAvailable = select(maxSockets, &readfds, NULL, NULL, &timeout) > 0; 
		
		if (dataIsAvailable)
		{			
			CheckForNewTcpPlayers();
			CheckForPlayerTraffic();			
		}
	}

	void TcpGameServerConnection::CheckForNewTcpPlayers()
	{
		if (FD_ISSET(listeningSocket, &readfds))
		{
			SOCKET connectedSocket;
			
			PeerInfo peerInfo;			
			connectedSocket = accept(listeningSocket, (struct sockaddr*)&peerInfo.Address, &peerInfo.Length);			

			if (connectedSocket != INVALID_SOCKET)
			{
				auto newPlayer = TcpNetworkPlayer(connectedSocket);
				Players.push_back(newPlayer); // Store incoming player sockets so we can listen for incoming player data too
				
				//eventManager->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
				_eventManager->RaiseEvent(_eventFactory->CreateNetworkPlayerJoinedEvent(newPlayer), this);
			}			
			
		}
	}

	std::vector<std::shared_ptr<Event>> TcpGameServerConnection::HandleEvent(std::shared_ptr<Event> evt)
	{
		return std::vector<std::shared_ptr<Event>>();
	}

	std::string TcpGameServerConnection::GetSubscriberName()
	{
		return "TcpGameServerConnection";
	}

	void TcpGameServerConnection::Create()
	{
		listeningSocket = networking->netTcpServer(host.c_str(), port.c_str());
	}

	void TcpGameServerConnection::CheckForPlayerTraffic()
	{
		for (size_t playerId = 0; playerId < Players.size(); playerId++)
		{
			if(!Players[playerId].IsConnected())
			{
				continue;
			}

			if (FD_ISSET(Players[playerId].GetSocket(), &readfds))
			{
				const int DEFAULT_BUFLEN = 512;
				char buffer[DEFAULT_BUFLEN];
				int bufferLength = DEFAULT_BUFLEN;
				ZeroMemory(buffer, bufferLength);

				// Read all incoming data
				int bytesReceived = networking->netReadVRec(Players[playerId].GetSocket(), buffer, bufferLength);

				if (bytesReceived > 0)
				{
					ParseReceivedPlayerPayload(playerId, buffer, bufferLength);
					RaiseNetworkTrafficReceievedEvent(buffer, playerId, bytesReceived);
				}

				if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
				{
					// Connection closed. Remove the socket from those being monitored for incoming traffic
					Players[playerId].SetIsConnected(false);

					auto result = shutdown(Players[playerId].GetSocket(), SD_BOTH);
					if (result == SOCKET_ERROR)
					{
						printf("shutdown failed: %d\n", WSAGetLastError());
						closesocket(Players[playerId].GetSocket());
						WSACleanup();
					}
				}
			}
		}
	}

	void TcpGameServerConnection::ParseReceivedPlayerPayload(const size_t& playerId, char* inPayload, int payloadLength)
	{
		if(!Players[playerId].IsConnected())
		{
			return;
		}

		auto msgHeader = serializationManager->GetMessageHeader(inPayload);
		auto messageType = msgHeader.MessageType;

		if(messageType == "ping")
		{	
			ProcessPingMessage(playerId);
		}
		else if(messageType == "requestPlayerDetails")
		{
			ProcessRequestPlayerDetailsMessage((int)playerId, msgHeader);
		}		
		else
		{
			// Send to other network players
			SendToConnectedPlayersExceptToSender(msgHeader.MessageTarget, inPayload, playerId);

			// Send to ourself
			auto event = serializationManager->Deserialize(msgHeader, inPayload);
			if(event)
			{
				_eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);
			}
		}

	}

	void TcpGameServerConnection::RaiseNetworkTrafficReceievedEvent(char buffer[512], const size_t& i, int bytesReceived)
	{
		auto event = _eventFactory->CreateNetworkTrafficReceivedEvent(buffer, std::to_string(i + 1), bytesReceived);

		_eventManager->RaiseEventWithNoLogging(event);
	}

	void TcpGameServerConnection::ProcessPingMessage(const size_t& playerId)
	{
		auto data = serializationManager->CreatePongMessage();
		networking->netSendVRec(Players[playerId].GetSocket(), data.c_str(), data.length());
	}

	void TcpGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage, const size_t& playerId)
	{
		for (auto player : Players)
		{
			if (!player.IsConnected() || player.GetNickName() == senderNickname)
			{
				continue;
			}

			networking->netSendVRec(Players[playerId].GetSocket(), serializedMessage.c_str(), serializedMessage.length());
		}
	}

	void TcpGameServerConnection::ProcessRequestPlayerDetailsMessage(int playerId, gamelib::MessageHeader& messageHeader)
	{
		// Record the players nickname
		Players[playerId].SetNickName(messageHeader.MessageTarget);
	}

	void TcpGameServerConnection::SendEventToAllPlayers(std::string serializedEvent)
	{		
		for (auto player : Players)
		{
			if (!player.IsConnected())
			{
				continue;
			}

			int sendResult = networking->netSendVRec(player.GetSocket(), serializedEvent.c_str(), serializedEvent.size());
		}
	}	
	
	TcpGameServerConnection::~TcpGameServerConnection()
	{
		for(auto player : Players)
		{			
			closesocket(player.GetSocket());
			Logger::Get()->LogThis("Closed player connection.");
		}
	}
}
