//
// Created by stuart on 11/01/2026.
//

#ifndef CPPGAMELIB_LOCALSOCKETPROTOCOLMANAGER_H
#define CPPGAMELIB_LOCALSOCKETPROTOCOLMANAGER_H
#include "IGameServerConnection.h"
#include "IGameClientConnetionProtocolManager.h"
#include "events/EventSubscriber.h"

namespace gamelib
{
    class LocalSocketProtocolManager : public IGameClientConnetionProtocolManager, public EventSubscriber
    {
    public:
        LocalSocketProtocolManager(const std::shared_ptr<IGameClientConnection> &gameClientConnection);

        void Connect(const char *address, const char *port) override;

        int Send(const char *data, int dataLength, unsigned long deltaMs) override;

        int Receive(char *receivedBuffer, int bufLength, unsigned long deltaMs) override;

        int SendAck(const Message &messageToAck, unsigned long sendTimeMs) override;

        bool Initialize() override;

        std::shared_ptr<IGameClientConnection> GetConnection() override;

        std::string GetSubscriberName() override;

        std::vector<std::shared_ptr<Event>>
        HandleEvent(const std::shared_ptr<Event> &evt, unsigned long deltaMs) override;
    private:
        std::shared_ptr<IGameClientConnection> gameClientConnection;
        std::shared_ptr<IGameServerConnection> gameServerConnection;
        constexpr static auto PackingBufferElements = 300;
        constexpr static auto ReceiveBufferMaxElements = 300;
        uint32_t packingBuffer[PackingBufferElements]{};
        uint32_t readBuffer[ReceiveBufferMaxElements]{};
    };
}

#endif //CPPGAMELIB_LOCALSOCKETPROTOCOLMANAGER_H