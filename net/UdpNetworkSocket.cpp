#include "UdpNetworkSocket.h"

gamelib::UdpNetworkSocket::UdpNetworkSocket(const SOCKET socket): socket(socket)
{
	
}

bool gamelib::UdpNetworkSocket::IsTcp()
{
	return false;
}

int gamelib::UdpNetworkSocket::Send(const char* data, const int dataLength)
{
	return send(socket, data, dataLength, 0);
}

int gamelib::UdpNetworkSocket::Receive(const char* readBuffer, const int bufLength)
{
	return recv(socket, const_cast<char*>(readBuffer), bufLength, 0); 
}

bool gamelib::UdpNetworkSocket::IsValid()
{
	return socket;
}

SOCKET gamelib::UdpNetworkSocket::GetRawSocket() const
{
	return socket;
}
