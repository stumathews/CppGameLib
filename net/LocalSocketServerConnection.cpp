//
// Created by stuart on 11/01/2026.
//

#include "LocalSocketServerConnection.h"

#include "PeerInfo.h"
#include "events/EventFactory.h"
#include "events/EventManager.h"
#include "file/Logger.h"
#include "file/SerializationManager.h"
#include <events/NetworkTrafficReceivedEvent.h>

gamelib::LocalSocketGameServerConnection::LocalSocketGameServerConnection(
    const char *socketPath, const Encoding wireFormat) : socketPath(socketPath),
                                                                  TheEncoding(wireFormat)
{

}

void gamelib::LocalSocketGameServerConnection::Initialize()
{
    this->networking = Networking::Get();
    this->eventFactory = EventFactory::Get();
    this->eventManager = EventManager::Get();
}

void gamelib::LocalSocketGameServerConnection::Create()
{
    listeningSocket = networking->netLocalSocketServer(socketPath);
}

void gamelib::LocalSocketGameServerConnection::CheckForPlayerTraffic(unsigned long deltaMs)
{
    if (FD_ISSET(listeningSocket, &readfds))
    {
        sockaddr_un from{};
        socklen_t from_len = sizeof(from);

        // Read all incoming data
        const int bytesReceived = recvfrom(listeningSocket, readBuffer, ReadBufferMaxElements,
                                           0, (sockaddr*)&from, &from_len);
        if (bytesReceived > 0)
        {
            // Raise event that the server received the client's data
            const auto event = eventFactory->CreateNetworkTrafficReceivedEvent(
                reinterpret_cast<const char *>(readBuffer), "LocalSocketGameClient", bytesReceived,
                                                                      this->GetSubscriberName());
            // Raise it
            eventManager->RaiseEventWithNoLogging(event);
        }

        // Send a response to the client
        const auto test = "Hello client, I'm the server";
        const auto sent = sendto(listeningSocket, test, sizeof(char) * strlen(test), 0,
                       (struct sockaddr*)&from, from_len);
        if (sent < 0)
        {
            perror("sendto");
            exit(1);
        }
    }
}

void gamelib::LocalSocketGameServerConnection::RaiseNetworkTrafficReceivedEvent(const char* buffer, const int bytesReceived,
                                                                   const PeerInfo &fromClient)
{


}


void gamelib::LocalSocketGameServerConnection::ProcessRequestPlayerDetailsMessage(const MessageHeader& messageHeader, const PeerInfo &fromClient)
{

}

void gamelib::LocalSocketGameServerConnection::Listen(unsigned long deltaMs)
{
    constexpr auto maxSockets = 5; // Number of pending connections to have in the queue at any one moment

    FD_ZERO(&readfds); // Clear the list of sockets that we are listening for/on
    FD_SET(listeningSocket, &readfds); // Add it to the list of file descriptors to listen for readability

    // Check on udp socket for incoming 'readable' data
    int max_fd = listeningSocket;
    const auto dataIsAvailable = select(max_fd + 1, &readfds, nullptr, nullptr, &timeout) > 0;

    if (dataIsAvailable)
    {
        std::cout << "Game server: data received\n";
        CheckForPlayerTraffic(deltaMs);
    }
}

void gamelib::LocalSocketGameServerConnection::ProcessPingMessage(PeerInfo fromClient)
{

}

void gamelib::LocalSocketGameServerConnection::ParseReceivedPlayerPayload(const char* inPayload, int payloadLength, const PeerInfo &fromClient)
{
}

void gamelib::LocalSocketGameServerConnection::SendToConnectedPlayersExceptToSender(const std::string& senderNickname, const std::string
                                                                   & serializedMessage)
{

}

int gamelib::LocalSocketGameServerConnection::  InternalSend(const SOCKET socket, const char *buf, const int len, const int flags,
                                                           const sockaddr *to, const int toLen)
{
    return sendto(socket, buf, len, flags, to, toLen);
}


void gamelib::LocalSocketGameServerConnection::SendEventToAllPlayers(const std::string serializedEvent)
{

}

void gamelib::LocalSocketGameServerConnection::Disconnect()
{
}

std::string gamelib::LocalSocketGameServerConnection::GetAddress()
{
    return socketPath;
}

std::string gamelib::LocalSocketGameServerConnection::GetPort()
{
    return "";
}

std::string gamelib::LocalSocketGameServerConnection::GetSubscriberName()
{
    return "LocalSocketGameServerConnection";
}

std::vector<std::shared_ptr<gamelib::Event>> gamelib::LocalSocketGameServerConnection::HandleEvent(
    const std::shared_ptr<Event> &evt, unsigned long deltaMs)
{
    return {};
}
