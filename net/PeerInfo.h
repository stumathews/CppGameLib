#pragma once
#include <WinSock2.h>

namespace gamelib
{
	class PeerInfo
	{
	public:
		PeerInfo(sockaddr_in address, int length)
		{
			this->Address = address;
			this->Length = sizeof(address);
		}

		PeerInfo()
		{
			this->Length = sizeof(Address);
			this->Address = {0};
		}
		sockaddr_in Address;
		int Length;
	};
}

