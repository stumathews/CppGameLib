#pragma once
#ifndef NETWORKSOCKETFACTORY_H
#define NETWORKSOCKETFACTORY_H

#include "TcpNetworkSocket.h"
#include "UdpConnectedNetworkSocket.h"
#include <memory>


namespace gamelib
{

	class NetworkConnectionFactory
	{
	public:

		// Creates either a tcp or udp 'connected' socket 
		static std::shared_ptr<IConnectedNetworkSocket> Connect(const bool isTcp, const char* address, const char* port)
		{
			auto networkSocket =  
				isTcp
				? std::dynamic_pointer_cast<IConnectedNetworkSocket>(std::make_shared<TcpNetworkSocket>(TcpNetworkSocket()))
				: std::dynamic_pointer_cast<IConnectedNetworkSocket>(std::make_shared<UdpConnectedNetworkSocket>(UdpConnectedNetworkSocket()));

			// Associate socket with address/port combination for the duration of its use
			networkSocket->Connect(address, port);

			return networkSocket;
		}
	};
}
#endif

