#pragma once
#ifndef GAMESERVERCONNECTIONFACTORY_H
#define GAMESERVERCONNECTIONFACTORY_H
#include <memory>

#include "IGameServerConnection.h"
#include "ReliableUdpGameServerConnection.h"
#include "TcpGameServerConnection.h"
#include "UdpGameServerConnection.h"
#include "utils/Utils.h"

namespace gamelib
{

	class GameServerConnectionFactory
	{
	public:
		static [[nodiscard]] std::shared_ptr<IGameServerConnection> Create(const bool isTcp, const std::string& address, const std::string& port, const bool useReliableUdp = false)
		{
			return isTcp ? To<IGameServerConnection>(std::make_shared<TcpGameServerConnection>(address, port))
						 : useReliableUdp
							? To<IGameServerConnection>(std::make_shared<ReliableUdpGameServerConnection>(address, port))
							: To<IGameServerConnection>(std::make_shared<UdpGameServerConnection>(address, port));
		}
	};
}

#endif