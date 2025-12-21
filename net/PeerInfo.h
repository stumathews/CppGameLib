#pragma once
#include <net/Networking.h>

namespace gamelib
{
	class PeerInfo
	{
	public:
		PeerInfo(const sockaddr_in address, int length)
		{
			this->Address = address;
			this->Length = sizeof(address);
		}

		PeerInfo()
		{
			this->Length = sizeof(Address);
			this->Address = {};
		}
		sockaddr_in Address{};
		socklen_t Length;
	};
}

