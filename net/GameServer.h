#pragma once
#include <memory>
#include <string>
#include <vector>
#include <common/Logger.h>
#include <WinSock2.h>

namespace gamelib
{
	class GameServer
	{
	public:
		GameServer(std::string address, std::string port);

		void Initialize();

		~GameServer();

		/// <summary>
		/// Checks to see if there is any data waiting to be read or written.
		/// </summary>
		void Listen();

		/// <summary>
		/// Check if there is any data waiting to be ready from any of the connected player sockets
		/// </summary>
		void CheckForPlayerTraffic();

		/// <summary>
		/// Write message back to the client
		/// </summary>
		/// <param name="i"></param>
		void ParsePayload(const size_t& playerId);		

		/// <summary>
		/// // New connection on main server listening socket?
		/// </summary>
		void CheckForNewPlayers();


		std::string address;
		std::string port;
		std::vector<SOCKET> Players;

	private:

		void RaiseNetworkTrafficReceievedEvent(char  buffer[512], const size_t& i, int bytesReceived);

		/// <summary>
		/// A set of file descriptors that are checked for writability
		/// </summary>
		fd_set readfds;
		SOCKET listeningSocket;

		/// <summary>
		/// // How long to wait for network data the arrive {0,0} means non-blocking
		/// </summary>
		struct timeval timeout;


	};
}

