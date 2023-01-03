#pragma once

#include "NetworkPlayer.h"
#include "PeerInfo.h"
#include <string>

namespace gamelib
{
	class UdpNetworkPlayer : public NetworkPlayer
	{
	public:
		UdpNetworkPlayer(const PeerInfo peerInfo, const std::string& nickName)
		{
			this->PeerInfo = peerInfo;
			SetNickName(nickName);
		}

		PeerInfo PeerInfo;		
	};
}
