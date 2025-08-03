#pragma once
#ifndef GAMESERVERCONNECTIONFACTORY_H
#define GAMESERVERCONNECTIONFACTORY_H
#include <memory>

#include "ReliableUdpGameServerConnection.h"
#include "TcpGameServerConnection.h"
#include "UdpGameServerConnection.h"
#include "utils/Utils.h"

namespace gamelib
{
	class IGameServerConnection;

	// creates connection to clients for game server
	class GameServerConnectionFactory
	{
	public:
		static [[nodiscard]] std::shared_ptr<IGameServerConnection> Create(
			const bool isTcp, const std::string& address, const std::string& port, const bool useReliableUdp = false,
			bool useEncryption = true, Encoding wireFormat = Encoding::json)
		{
			return isTcp ? To<IGameServerConnection>(std::make_shared<TcpGameServerConnection>(address, port, wireFormat))
						 : useReliableUdp
							? To<IGameServerConnection>(
							 std::make_shared<
								 ReliableUdpGameServerConnection>(address, port, useEncryption, wireFormat))
							: To<IGameServerConnection>(std::make_shared<UdpGameServerConnection>(address, port, wireFormat));
		}
	};
}

#endif