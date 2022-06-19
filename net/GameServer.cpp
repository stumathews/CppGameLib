#include "GameServer.h"
#include <events/EventManager.h>
#include <net/PeerInfo.h>
#include <events/PlayerNetworkTrafficRecievedEvent.h>
#include <net/Networking.h>
namespace gamelib
{
	GameServer::GameServer(std::string address, std::string port)
	{
		this->address = address;
		this->port = port;		
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
		int networkAvailabilityResult = -1; // -1 means is error

		// Clear the list of sockets that we are listening for/on
		FD_ZERO(&readfds);
		
		// Add it to the list of file descriptors to listen for readability
		FD_SET(listeningSocket, &readfds);

		// Also listen for any traffic on any of the player's sockets
		for(auto playerSocket : Players)
		{
			FD_SET(playerSocket, &readfds);
		}

		// How long to wait for network data the arrive {0,0} means non-blocking
		struct timeval noDataTimeout;
		noDataTimeout.tv_sec = 0;
		noDataTimeout.tv_usec = 0;		

		// Check monitored sockets for incoming 'readable' data
		if (networkAvailabilityResult = select(maxSockets, &readfds, NULL, NULL, &noDataTimeout))
		{
			// Check to see if the activity was on one of our listening file descriptors
			
			// New connection on listening socket?
			if (FD_ISSET(listeningSocket, &readfds))
			{
				// Yes, 
				PeerInfo peerInfo;
				SOCKET connectedSocket = accept(listeningSocket, (struct sockaddr*)&peerInfo.Address, &peerInfo.Length);
				if(connectedSocket == INVALID_SOCKET)
				{
					// That socket was not right...
					return;
				}
				// Store incoming player sockets so we can listen for incoming player data too
				Players.push_back(connectedSocket);
								
				// Tell someone that a player conencted to the game server
				EventManager::Get()->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
			}			
			else
			{
				// No, Must data on another socket...

				// Any of our players sending us data?
				for(size_t i = 0; i < Players.size(); i++)
				{
					// Check player's socket for data availability
					if(FD_ISSET(Players[i], &readfds))
					{			
						// Yes, this player sent us data...						

						// Read the data:

						const int DEFAULT_BUFLEN = 512;
						char buffer[DEFAULT_BUFLEN];
						int bufferLength = DEFAULT_BUFLEN;
						ZeroMemory(buffer, bufferLength);

						// Read off the network, wait for all the data
						int bytesReceived = recv(Players[i], buffer, bufferLength, 0);
						
						if(bytesReceived > 0)
						{						
							// We successfully read some data... 

							// Raise event telling interested parties about this event
							auto event = std::shared_ptr<gamelib::PlayerNetworkTrafficRecievedEvent>(new PlayerNetworkTrafficRecievedEvent(gamelib::EventType::NetworkPlayerTrafficReceived, 0));
							event->Message = buffer;
							event->Identifier = std::to_string(i + 1);
							event->bytesReceived = bytesReceived;

							EventManager::Get()->RaiseEventWithNoLogging(event);

							// TODO: Send this to the other players also
							for(auto socket : Players)
							{
								
							}
						}
						else if(bytesReceived == SOCKET_ERROR && WSAGetLastError() == WSAECONNRESET)
						{
							// Connection closed. Remove the socket from those being monitored for incoming traffic
							Players.erase(begin(Players)+i);
						}
					}
				}
			}
		}
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
