#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/PlayerMovedEvent.h>
#include <events/EventFactory.h>
#include <SerializationManager.h>
#include <net/MessageHeader.h>

using namespace json11;


namespace gamelib
{
	GameServer::GameServer(std::string address, std::string port)
	{
		this->address = address;
		this->port = port;	
		this->timeout.tv_sec = 0;
		this->timeout.tv_usec = 0;
		this->serializationManager = nullptr;
		this->eventManager = nullptr;
		this->networking = nullptr;
		this->eventFactory = nullptr;
	}

	void GameServer::Initialize()
	{
		Logger::Get()->LogThis("Game Server Starting.");

		// Create a new server socket connection
		listeningSocket = Networking::Get()->netTcpServer(address.c_str(), port.c_str());

		// Read client nickname
		this->nickname  = SettingsManager::Get()->GetString("networking", "nickname");

		this->serializationManager = SerializationManager::Get();
		this->eventManager = EventManager::Get();
		this->networking = Networking::Get();
		this->eventFactory = EventFactory::Get();

		// Send our events off on the network
		eventManager->SubscribeToEvent(EventType::PlayerMovedEventType, this);
		eventManager->SubscribeToEvent(EventType::ControllerMoveEvent, this);
		eventManager->SubscribeToEvent(EventType::Fire, this);
		
	}
	
	void GameServer::Listen()
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
			CheckForNewPlayers();				
			CheckForPlayerTraffic();			
		}
	}

	void GameServer::CheckForNewPlayers()
	{
		if (FD_ISSET(listeningSocket, &readfds))
		{
			PeerInfo peerInfo;
			SOCKET connectedSocket = accept(listeningSocket, (struct sockaddr*)&peerInfo.Address, &peerInfo.Length);

			if (connectedSocket != INVALID_SOCKET)
			{					
				auto requestPlayerDetailsMessage = serializationManager->CreateRequestPlayerDetailsMessage();
				networking->netSendVRec(connectedSocket, requestPlayerDetailsMessage.c_str(), requestPlayerDetailsMessage.length());

				Players.push_back(NetworkPlayer(connectedSocket)); // Store incoming player sockets so we can listen for incoming player data too
				
				eventManager->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
			}
		}
	}

	void GameServer::CheckForPlayerTraffic()
	{
		for (size_t playerId = 0; playerId < Players.size(); playerId++)
		{
			if(!Players[playerId].IsConnected())
			{
				continue;
			}

			if (FD_ISSET(Players[playerId].GetSocket(), &readfds))
			{
				// Yes, player data is available
				
				const int DEFAULT_BUFLEN = 512;
				char buffer[DEFAULT_BUFLEN];
				int bufferLength = DEFAULT_BUFLEN;
				ZeroMemory(buffer, bufferLength);

				ZeroMemory(buffer, bufferLength);

				// Read all incoming data
				int bytesReceived = networking->netReadVRec(Players[playerId].GetSocket(), buffer, bufferLength);

				if (bytesReceived > 0)
				{
					ParseReceivedPlayerPayload(playerId, buffer, bufferLength);

					RaiseNetworkTrafficReceievedEvent(buffer, playerId, bytesReceived);

				}
				else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
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

	void GameServer::ParseReceivedPlayerPayload(const size_t& playerId, char* inPayload, int payloadLength)
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
				eventManager->DispatchEventToSubscriber(event, msgHeader.MessageTarget);
			}
		}

	}	

	void GameServer::ProcessRequestPlayerDetailsMessage(int playerId, gamelib::MessageHeader& messageHeader)
	{
		// Record the players nickname
		Players[playerId].SetNickName(messageHeader.MessageTarget);
	}

	void GameServer::ProcessPingMessage(const size_t& playerId)
	{
		auto data = serializationManager->CreatePongMessage();
		networking->netSendVRec(Players[playerId].GetSocket(), data.c_str(), data.length());
	}

	void GameServer::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage, const size_t& playerId)
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

	void GameServer::RaiseNetworkTrafficReceievedEvent(char buffer[512], const size_t& i, int bytesReceived)
	{
		auto event = eventFactory->CreateNetworkTrafficReceivedEvent(buffer, std::to_string(i + 1), bytesReceived);

		eventManager->RaiseEventWithNoLogging(event);
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(std::shared_ptr<Event> evt)
	{				
		SendEventToAllConnectedPlayers(serializationManager->Serialize(evt, nickname)); 

		return std::vector<std::shared_ptr<Event>>();
	}

	void GameServer::SendEventToAllConnectedPlayers(std::string serializedEvent)
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

	std::string GameServer::GetSubscriberName()
	{
		return "Game Server";
	}
		
	GameServer::~GameServer()
	{
		for(auto player : Players)
		{			
			closesocket(player.GetSocket());
			Logger::Get()->LogThis("Closed player connection.");
		}
	}
}
