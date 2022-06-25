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
	}

	void GameServer::Initialize()
	{
		Logger::Get()->LogThis("Game Server Starting.");

		// Create a new server socket connection
		listeningSocket = Networking::Get()->netTcpServer(address.c_str(), port.c_str());

		// Read client nickname
		this->nickname  = SettingsManager::Get()->GetString("networking", "nickname");

		EventManager::Get()->SubscribeToEvent(gamelib::EventType::PlayerMovedEventType, this);
		
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
		// New connection on listening socket?
		if (FD_ISSET(listeningSocket, &readfds))
		{
			PeerInfo peerInfo;
			SOCKET connectedSocket = accept(listeningSocket, (struct sockaddr*)&peerInfo.Address, &peerInfo.Length);

			if (connectedSocket != INVALID_SOCKET)
			{					
				auto requestPlayerDetailsMessage = SerializationManager::Get()->CreateRequestPlayerDetailsMessage();
				Networking::Get()->netSendVRec(connectedSocket, requestPlayerDetailsMessage.c_str(), requestPlayerDetailsMessage.length());

				Players.push_back(NetworkPlayer(connectedSocket)); // Store incoming player sockets so we can listen for incoming player data too
				EventManager::Get()->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
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
				int bytesReceived = Networking::Get()->netReadVRec(Players[playerId].GetSocket(), buffer, bufferLength);

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

		auto msgHeader = SerializationManager::Get()->GetMessageHeader(inPayload);
		auto messageType = msgHeader.MessageType;
		auto serializationManager = SerializationManager::Get();
		auto eventManager = EventManager::Get();

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
			auto event = serializationManager->DeserializeToEvent(msgHeader, inPayload);
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
		/*

		Json payload = Json::object
		{
		{ "messageType", "ping" },
		{ "isHappy", false },
		{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
		{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
		{ "ages", Json::array{ 1, 2, 3 } },
		{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		*/

		auto data = SerializationManager::Get()->CreatePongMessage();
		Networking::Get()->netSendVRec(Players[playerId].GetSocket(), data.c_str(), data.length());
	}

	void GameServer::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, std::string serializedMessage, const size_t& playerId)
	{
		for (auto player : Players)
		{
			if (!player.IsConnected() || player.GetNickName() == senderNickname)
			{
				continue;
			}

			Networking::Get()->netSendVRec(Players[playerId].GetSocket(), serializedMessage.c_str(), serializedMessage.length());
		}
	}

	void GameServer::RaiseNetworkTrafficReceievedEvent(char buffer[512], const size_t& i, int bytesReceived)
	{
		// Raise event telling interested parties about this event
		auto event = std::shared_ptr<gamelib::NetworkTrafficRecievedEvent>(new NetworkTrafficRecievedEvent(gamelib::EventType::NetworkTrafficReceived, 0));
		event->Message = buffer;
		event->Identifier = std::to_string(i + 1);
		event->bytesReceived = bytesReceived;

		EventManager::Get()->RaiseEventWithNoLogging(event);
	}

	std::vector<std::shared_ptr<Event>> GameServer::HandleEvent(std::shared_ptr<Event> evt)
	{
		SendEventToAllConnectedPlayers(EventFactory::Get()->Serialize(evt, nickname));

		auto noCreatedEvents = std::vector<std::shared_ptr<Event>>();
		return noCreatedEvents;
	}

	void GameServer::SendEventToAllConnectedPlayers(std::string serializedEvent)
	{
		for (auto player : Players)
		{
			if (!player.IsConnected())
			{
				continue;
			}
			int sendResult = Networking::Get()->netSendVRec(player.GetSocket(), serializedEvent.c_str(), serializedEvent.size());
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
