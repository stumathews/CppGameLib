#pragma once
#include <string>

namespace gamelib
{
	class Connection
	{
	public:
		Connection(const std::string& host, const std::string& port);

		[[nodiscard]] bool IsConnected() const
		{
			return isConnected;
		}

		static void Connect()
		{

		}

		static char* Read()
		{
			const auto nothing = "nothing";
			return const_cast<char*>(nothing);
		}
	private:
		bool isConnected{};
		std::string host, port;
	};
}

