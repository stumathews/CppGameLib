#pragma once
#include <string>

namespace gamelib
{
	class NetworkPlayer
	{
	public:
		void SetNickName(const std::string& nick);

		std::string GetNickName();

	protected:	
			std::string nickName;
	};
}