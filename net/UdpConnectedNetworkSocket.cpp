#include "UdpConnectedNetworkSocket.h"

#include "Networking.h"

gamelib::UdpConnectedNetworkSocket::UdpConnectedNetworkSocket(): socket(0)
{
	
}

void gamelib::UdpConnectedNetworkSocket::Connect(const char* address, const char* port)
{
	auto* networking = Networking::Get();
	socket = networking->netConnectedUdpClient(address, port);
}

bool gamelib::UdpConnectedNetworkSocket::IsTcp()
{
	return false;
}

int gamelib::UdpConnectedNetworkSocket::Send(const char* data, const int dataLength)
{
	return send(socket, data, dataLength, 0);
}

int gamelib::UdpConnectedNetworkSocket::Receive(char* readBuffer, const int bufLength)
{
	return recv(socket, readBuffer, bufLength, 0); 
}

bool gamelib::UdpConnectedNetworkSocket::IsValid()
{
	return socket;
}

SOCKET gamelib::UdpConnectedNetworkSocket::GetRawSocket() const
{
	return socket;
}
