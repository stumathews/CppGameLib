#include "TcpNetworkSocket.h"

#include "Networking.h"

bool gamelib::TcpNetworkSocket::IsTcp()
{
	return true;
}

int gamelib::TcpNetworkSocket::Send(const char* data, const int dataLength, const gamelib::DeltaTime* deltaTime)
{
	return Networking::netSendVRec(socket, data, dataLength);
}

int gamelib::TcpNetworkSocket::Receive(char* readBuffer, const int bufLength, const gamelib::DeltaTime* deltaTime)
{
	return Networking::netReadVRec(socket, readBuffer, bufLength);
}

bool gamelib::TcpNetworkSocket::IsValid()
{
	return socket;
}

SOCKET gamelib::TcpNetworkSocket::GetRawSocket() const
{
	return socket;
}

void gamelib::TcpNetworkSocket::Connect(const char* address, const char* port)
{
	auto* networking = Networking::Get();
	socket = networking->netTcpClient(address, port);
}
