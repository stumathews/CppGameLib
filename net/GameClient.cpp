#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <net/PeerInfo.h>
#include <events/NetworkTrafficRecievedEvent.h>
#include <events/EventManager.h>
#include <json/json11.h>

using namespace json11;

namespace gamelib
{
	GameClient::~GameClient()
	{
		auto result = shutdown(clientSocket, SD_SEND);
		if (result == SOCKET_ERROR) 
		{
			printf("shutdown failed: %d\n", WSAGetLastError());
			closesocket(clientSocket);
			WSACleanup();
		}
	}

	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message; message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;
		Logger::Get()->LogThis(message.str());

		// Make a connection to the game server.
		clientSocket = Networking::Get()->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str());
		
		if(clientSocket)
		{
			this->IsDiconnectedFromGameServer = false;
		}
	}

	void GameClient::PingGameServer()
	{
		const char payload[] = "Ping!";
		
		int sendResult = Networking::Get()->netSendVRec(clientSocket, payload, strlen(payload));

		if (sendResult == SOCKET_ERROR) 
		{
			Networking::Get()->netError(0,0, "Ping Game server connect failed. Shutting down client");
			closesocket(clientSocket);
			WSACleanup();
		}
	}

	void GameClient::Listen()
	{
		if(this->IsDiconnectedFromGameServer)
		{
			return;
		}

		const auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment
		int networkAvailabilityResult = -1; // -1 means is error

		// Clear the list of sockets that we are listening for/on
		FD_ZERO(&readfds);
		
		// Add it to the list of file descriptors to listen for readability
		FD_SET(clientSocket, &readfds);

		// How long to wait for network data the arrive {0,0} means non-blocking
		struct timeval noDataTimeout;
		noDataTimeout.tv_sec = 0;
		noDataTimeout.tv_usec = 0;		

		// Check monitored sockets for incoming 'readable' data
		if (networkAvailabilityResult = select(maxSockets, &readfds, NULL, NULL, &noDataTimeout))
		{
			// Check to see if the activity was on one of our listening file descriptors
			
			// New connection on listening socket?
			if (FD_ISSET(clientSocket, &readfds))
			{
				const int DEFAULT_BUFLEN = 512;
				char buffer[DEFAULT_BUFLEN];
				int bufferLength = DEFAULT_BUFLEN;
				ZeroMemory(buffer, bufferLength);

				// Read the payload off the network, wait for all the data
				int bytesReceived = Networking::Get()->netReadVRec(clientSocket, buffer, bufferLength);
						
				if(bytesReceived > 0)
				{						
					// We successfully read some data... 
					
					// Parse the payload to find out what kind of message it is (refer to protocol)
					std::string error;
					auto json = Json::parse(buffer, error);
					/*
					
					Json my_json = Json::object {
									{ "message", "pong" },
									{ "isHappy", false },
									{ "names", Json::array { "Stuart", "Jenny", "bruce" } },
									{ "ages", Json::array { 1, 2, 3} },
									{ "fish", Json::object { { "yo", "sushi"}}}
							};

					
					*/
					
					auto message = json["message"].string_value();
					auto jennyAge = json["ages"][1].int_value();
					auto bruceAge = json["ages"][2].int_value();
					auto ages = json["ages"].array_items();
					auto fish = json["fish"].object_items();

					auto event = std::shared_ptr<gamelib::NetworkTrafficRecievedEvent>(new NetworkTrafficRecievedEvent(gamelib::EventType::NetworkTrafficReceived, 0));
					event->Message = buffer;
					event->Identifier = "Game Server";
					event->bytesReceived = bytesReceived;


					EventManager::Get()->RaiseEventWithNoLogging(event);

					// Distribute to EventManager...

					this->IsDiconnectedFromGameServer = false;
				}
				else if(bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
				{
					// Connection closed. Server died.
					this->IsDiconnectedFromGameServer = true;
				}
			}
		}
	}
}
