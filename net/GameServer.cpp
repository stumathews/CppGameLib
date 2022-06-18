#include "GameServer.h"
#include <events/EventManager.h>
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
		// How long to wait for network data ta arrive
		struct timeval noDataTimeout;
		noDataTimeout.tv_sec = 0;
		noDataTimeout.tv_usec = 0; /*1/4 of a second*/ /* 1000000 microseconds in a second, 1000 usecs in a milisecond*/

		// Add it to the list of file descriptors to listen for readability
		FD_ZERO(&readfds);
		FD_SET(listeningSocket, &readfds);

		const auto maxSockets = 5;

		// Check for data on network sockets within dataAvailableSockets
		auto result = select(maxSockets, &readfds, NULL, NULL, &noDataTimeout);
		//auto result = select(maxSockets, &readfds, NULL, NULL, NULL);

		// Evaluate if we got data or we timed out (received none)
		if (result)
		{
			struct sockaddr_in peer;
			int peerlen = sizeof(peer);

			// Check to see if the activity was on one of our listening file descriptors
			// New connection on listening socket?
			if (FD_ISSET(listeningSocket, &readfds))
			{
				// Yes, 
				int peerlen;
				struct sockaddr_in peer;
				peerlen = sizeof(peer);

				// This is blocking, but we've already got the socket so we dont mind
				SOCKET connected_socket = accept(listeningSocket, (struct sockaddr*)&peer, &peerlen);

				// Store incoming player connections
				Players.push_back(connected_socket);
								
				EventManager::Get()->RaiseEventWithNoLogging(std::make_shared<gamelib::Event>(gamelib::EventType::NetworkPlayerJoined));
			}
			else
			{
				// Not our socket, timed out or error
				if(result == SOCKET_ERROR)
				{
					int errorCode = WSAGetLastError();
					wchar_t *s = NULL;
					FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
								   NULL, WSAGetLastError(),
								   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
								   (LPWSTR)&s, 0, NULL);
					fprintf(stderr, "%S\n", s);
					LocalFree(s);
				}
				if(result == 0)
				{
				// Timeout occured
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
