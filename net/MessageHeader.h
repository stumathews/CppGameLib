#pragma once
#include <string>

namespace gamelib
{
	class MessageHeader
	{
	public:
		std::string MessageType;
		std::string MessageTarget;
	};
}
