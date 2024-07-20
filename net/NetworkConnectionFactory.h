#pragma once
#ifndef NETWORKSOCKETFACTORY_H
#define NETWORKSOCKETFACTORY_H

#include "TcpNetworkSocket.h"
#include "UdpConnectedNetworkSocket.h"
#include <memory>

#include "ReliableUdpNetworkSocket.h"


namespace gamelib
{

	class NetworkConnectionFactory
	{
	public:

		static std::shared_ptr<IConnectedNetworkSocket> Create(const bool isTcp, const bool useReliableUdp = false)
		{
			auto networkSocket =  
				isTcp
				? std::dynamic_pointer_cast<IConnectedNetworkSocket>(std::make_shared<TcpNetworkSocket>(TcpNetworkSocket()))
				: useReliableUdp
					? std::dynamic_pointer_cast<IConnectedNetworkSocket>(std::make_shared<ReliableUdpNetworkSocket>(ReliableUdpNetworkSocket()))
					: std::dynamic_pointer_cast<IConnectedNetworkSocket>(std::make_shared<UdpConnectedNetworkSocket>(UdpConnectedNetworkSocket()));

			return networkSocket;
		}		
	};
}
#endif

