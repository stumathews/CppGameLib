#pragma once
#ifndef NETWORKSOCKETFACTORY_H
#define NETWORKSOCKETFACTORY_H

#include <WinSock2.h>

#include "net/Networking.h"
#include "net/NetworkManager.h"
#include "TcpNetworkSocket.h"
#include "UdpNetworkSocket.h"
#include <memory>


namespace gamelib
{

	class NetworkConnectionFactory
	{
	public:
		static std::shared_ptr<INetworkSocket> Connect(const bool isTcp, const char* address, const char* port)
		{
			auto* networking = Networking::Get();


			const SOCKET socket = isTcp ? networking->netTcpClient(address, port)
				                      : networking->netConnectedUdpClient(address, port);

			return isTcp ? std::dynamic_pointer_cast<INetworkSocket>(std::make_shared<TcpNetworkSocket>(TcpNetworkSocket(socket)))
				: std::dynamic_pointer_cast<INetworkSocket>(std::make_shared<UdpNetworkSocket>(UdpNetworkSocket(socket)));
		}
	};
}
#endif

