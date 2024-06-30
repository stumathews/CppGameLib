#include "TcpNetworkSocket.h"

bool gamelib::TcpNetworkSocket::IsTcp()
{
	return true;
}

int gamelib::TcpNetworkSocket::Send(const char* data, const int dataLength) const
{
	return Networking::netSendVRec(socket, data, dataLength);
}

int gamelib::TcpNetworkSocket::Receive(const char* readBuffer, const int bufLength) const
{
	return Networking::netReadVRec(socket, const_cast<char*>(readBuffer), bufLength);
}

SOCKET gamelib::TcpNetworkSocket::GetRawSocket() const
{
	return socket;
}
