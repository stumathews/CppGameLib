#pragma once
#include <string>

namespace gamelib
{
	class MessageHeader
	{
	public:
		std::string TheMessageType;
		std::string MessageTarget;
	};
}
