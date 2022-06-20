#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <net/Networking.h>
#include <json/json11.h>

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
	}
	
	void GameServer::Listen()
	{		
		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		
		FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on			
		FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability

		// Also listen for any traffic on any of the player's sockets
		for(auto playerSocket : Players)
		{
			FD_SET(playerSocket, &readfds);
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
				Players.push_back(connectedSocket); // Store incoming player sockets so we can listen for incoming player data too
				EventManager::Get()->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
			}
		}
	}

	void GameServer::CheckForPlayerTraffic()
	{
		for (size_t playerId = 0; playerId < Players.size(); playerId++)
		{
			if (FD_ISSET(Players[playerId], &readfds))
			{
				// Yes, player data is available
				
				const int DEFAULT_BUFLEN = 512;
				char buffer[DEFAULT_BUFLEN];
				int bufferLength = DEFAULT_BUFLEN;
				ZeroMemory(buffer, bufferLength);

				ZeroMemory(buffer, bufferLength);

				// Read all incoming data
				int bytesReceived = Networking::Get()->netReadVRec(Players[playerId], buffer, bufferLength);

				if (bytesReceived > 0)
				{
					ParsePayload(playerId);

					RaiseNetworkTrafficReceievedEvent(buffer, playerId, bytesReceived);

				}
				else if (bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
				{
					// Connection closed. Remove the socket from those being monitored for incoming traffic
					Players.erase(begin(Players) + playerId);
				}
			}
		}
	}

	void GameServer::ParsePayload(const size_t& playerId)
	{
		// TODO: Parse the message, determine what type of message it is and then serialize into an event 
		// TODO: suitable for adding to the event queue, taking care to label who the event is from and who it is for.

		// Write message back to the client
		Json payload = Json::object{
			{ "message", "pong" },
			{ "isHappy", false },
			{ "eventType", (int)gamelib::EventType::NetworkTrafficReceived },
			{ "names", Json::array{ "Stuart", "Jenny", "bruce" } },
			{ "ages", Json::array{ 1, 2, 3 } },
			{ "fish", Json::object{ { "yo", "sushi" } } }
		};

		auto json_str = payload.dump();

		// Send generic reseponse
		Networking::Get()->netSendVRec(Players[playerId], json_str.c_str(), json_str.length());
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

	
	

	GameServer::~GameServer()
	{
		for(auto playerConnection : Players)
		{			
			closesocket(playerConnection);
			Logger::Get()->LogThis("Closed player connection.");
		}
	}
}
