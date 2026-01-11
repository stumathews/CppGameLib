//
// Created by stuart on 11/01/2026.
//

#include "LocalNetworkSocket.h"

LocalNetworkSocket::LocalNetworkSocket(): socket(0)
{

}

bool LocalNetworkSocket::IsTcp()
{
    return false;
}

int LocalNetworkSocket::Send(const char *data, int dataLength, unsigned long deltaMs)
{


    return send(socket, data, dataLength, 0);
}

int LocalNetworkSocket::Receive(char *readBuffer, int bufLength, unsigned long deltaMs)
{
    return recv(socket, readBuffer, bufLength, 0);
}

bool LocalNetworkSocket::IsValid()
{
    return socket;
}

SOCKET LocalNetworkSocket::GetRawSocket() const
{
    return socket;
}

void LocalNetworkSocket::Connect(const char *socketPath, const char* unused)
{
    const auto* networking = gamelib::Networking::Get();
    socket = networking->netLocalSocketClient(socketPath);
    this->socketPath = socketPath;
}
