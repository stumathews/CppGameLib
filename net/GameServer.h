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


		std::string address;
		std::string port;
		std::vector<SOCKET> Players;

	private:

		/// <summary>
		/// A set of file descriptors that are checked for writability
		/// </summary>
		fd_set readfds;
		SOCKET listeningSocket;

	};
}

