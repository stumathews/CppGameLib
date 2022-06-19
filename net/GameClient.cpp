#include "GameClient.h"
#include "Networking.h"
#include <sstream>
#include <net/PeerInfo.h>
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
		std::stringstream message;
		message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;

		Logger::Get()->LogThis(message.str());

		// Make a connection to the game server.
		clientSocket = Networking::Get()->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str());
	}

	void GameClient::PingGameServer()
	{
		const char ping[] = "Ping!";
		int sendResult = send(clientSocket, ping, (int) strlen(ping), 0);
		
		// If there was an error sending, shutdown our side and thats that!
		if (sendResult == SOCKET_ERROR) 
		{
			Networking::Get()->netError(0,0, "Ping Game server connect failed");
			closesocket(clientSocket);
			WSACleanup();
		}
	}

	void GameClient::Listen()
	{
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
				// Yes, Read some and send the event off to the event mangager for processing
			}
		}
	}
}
