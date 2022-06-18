#pragma once
#include <string>

namespace gamelib
{
	class Connection
	{
	public:
		Connection(std::string host, std::string port);
		bool IsConnected()
		{
			return isConnected;
		}

		void Connect()
		{

		}

		char* Read()
		{
			const char* nothing = "nothing";
			return (char*) nothing;
		}
	private:
		bool isConnected;
		std::string host, port;
	};
}

