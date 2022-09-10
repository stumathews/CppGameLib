#include "NetworkPlayer.h"

namespace gamelib
{

	void NetworkPlayer::SetNickName(const std::string nick)
	{
		NickName = nick;
	}

	std::string NetworkPlayer::GetNickName()
	{
		return NickName;
	}
}