//
// Created by stuart on 11/01/2026.
//

#include "LocalSocketProtocolManager.h"

#include "IGameClientConnection.h"

gamelib::LocalSocketProtocolManager::LocalSocketProtocolManager(
    const std::shared_ptr<IGameClientConnection> &gameClientConnection) : gameClientConnection(gameClientConnection)
{  }

void gamelib::LocalSocketProtocolManager::Connect(const char *address, const char *port)
{
    gameClientConnection->Connect(address, port);
}

int gamelib::LocalSocketProtocolManager::Send(const char *data, int dataLength, unsigned long deltaMs)
{
    return gameClientConnection->Send(data, dataLength);
}

int gamelib::LocalSocketProtocolManager::Receive(char *receivedBuffer, int bufLength, unsigned long deltaMs)
{
    const int bytesReceived = gameClientConnection->Receive(receivedBuffer, bufLength, deltaMs);
    return bytesReceived;
}

int gamelib::LocalSocketProtocolManager::SendAck(const Message &messageToAck, unsigned long sendTimeMs)
{
    const auto sendResult = gameClientConnection->Send(reinterpret_cast<char*>(packingBuffer), 99);
    return sendResult;
}

bool gamelib::LocalSocketProtocolManager::Initialize()
{
    return true;
}

std::shared_ptr<gamelib::IGameClientConnection> gamelib::LocalSocketProtocolManager::GetConnection()
{
    return gameClientConnection;
}

std::string gamelib::LocalSocketProtocolManager::GetSubscriberName()
{
    return "LocalSocketProtocolManager";
}

std::vector<std::shared_ptr<gamelib::Event>> gamelib::LocalSocketProtocolManager::HandleEvent(
    const std::shared_ptr<Event> &evt, unsigned long deltaMs)
{
    return {};
}

