#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <net/Networking.h>
#include <json/json11.h>
#include <events/PlayerMovedEvent.h>

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
				// Request player details
				Json sendPayload = Json::object 
				{
					{ "messageType", "requestPlayerDetails" }
				};

				auto json_str = sendPayload.dump();
				Networking::Get()->netSendVRec(connectedSocket, json_str.c_str(), json_str.length());

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

					// Perhaps not shutting down like if a security flaw!
					// TODO: Consider taking out for thesis
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

		// TODO: Parse the message, determine what type of message it is and then serialize into an event 
		// TODO: suitable for adding to the event queue, taking care to label who the event is from and who it is for.

		// Parse the payload to find out what kind of message it is (refer to protocol)
		std::string error;
		auto parsedJson = Json::parse(inPayload, error);

		auto messageType = parsedJson["messageType"].string_value();

		if(messageType == "ping")
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

			Json sendPayload = Json::object{
				{ "messageType", "pong" },
				{ "isHappy", true },
				{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
				{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
				{ "ages", Json::array{ 1, 2, 3 } },
				{ "fish", Json::object{ { "yo", "sushi" } } }
			};

			auto json_str = sendPayload.dump();
			Networking::Get()->netSendVRec(Players[playerId].GetSocket(), json_str.c_str(), json_str.length());
		}
		else if(messageType == "requestPlayerDetails")
		{
			Players[playerId].SetNickName(parsedJson["Nickname"].string_value());
		}

		// Tell everyone (including myself) that someone (Player) has told me (GameServer) they moved!
		// Radiate copies to all players except the player
		else
		{
			const auto& senderNickname = parsedJson["nickname"].string_value();

			// Send this onto our EventManager but targetting the player with the specified nickname 
			
			// TODO: Deserialize into Event and put onto EventManager

			// AND
			// Send it to all the other players but not to the original sender
			for(auto player : Players)
			{
				if(!player.IsConnected() || player.GetNickName() == senderNickname)
				{
					continue;
				}

				auto json_str = parsedJson.dump();
				Networking::Get()->netSendVRec(Players[playerId].GetSocket(), json_str.c_str(), json_str.length());
			}
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
		/* Schedule our events that have occured to be sent to all the connected players */
		auto createdEvents = std::vector<std::shared_ptr<Event>>();

		switch(evt->type)
		{
			case gamelib::EventType::PlayerMovedEventType:
				// our player moved.
				auto playerMovedEvent = std::dynamic_pointer_cast<PlayerMovedEvent>(evt);
				Json payload = Json::object
				{
					{ "messageType", ToString(evt->type) },
					{ "direction", ToString(playerMovedEvent->direction) },
					{ "nickname", nickname }
				};

				auto json_str = payload.dump();

				// Duplex the message to all players
				for(auto player : Players)
				{
					if(!player.IsConnected())
					{
						continue;
					}
					int sendResult = Networking::Get()->netSendVRec(player.GetSocket(), json_str.c_str(), json_str.size());
				}
				
			break;
		}

		return createdEvents;

	}

	std::string GameServer::GetSubscriberName()
	{
		return nickname;
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
