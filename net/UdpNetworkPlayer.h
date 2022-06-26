#pragma once
#include <net/PeerInfo.h>
#include <string>
namespace gamelib
{
	class UdpNetworkPlayer
	{
	public:
		UdpNetworkPlayer(PeerInfo peerInfo, std::string nickName)
		{
			this->peerInfo = peerInfo;
			this->NickName = nickName;
		}

		PeerInfo peerInfo;		
		std::string NickName;
	};
}
