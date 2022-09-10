#pragma once

#include "NetworkPlayer.h"
#include "PeerInfo.h"
#include <string>

namespace gamelib
{
	class UdpNetworkPlayer : public gamelib::NetworkPlayer
	{
	public:
		UdpNetworkPlayer(PeerInfo peerInfo, std::string nickName)
		{
			this->peerInfo = peerInfo;
			SetNickName(nickName);
		}

		PeerInfo peerInfo;		
	};
}
