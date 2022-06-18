#include "GameClient.h"
#include <sstream>
namespace gamelib
{
	void GameClient::Connect(std::shared_ptr<GameServer> gameServer)
	{
		std::stringstream message;
		message << "Connecting to game server: " << gameServer->address << ":" << gameServer->port;

		Logger::Get()->LogThis(message.str());

		clientSocket = Networking::Get()->netTcpClient(gameServer->address.c_str(), gameServer->port.c_str());
	}
}
