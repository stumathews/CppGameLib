//
// Created by stuart on 11/01/2026.
//

#ifndef CPPGAMELIB_LOCALSOCKETSERVERCONNECTION_H
#define CPPGAMELIB_LOCALSOCKETSERVERCONNECTION_H
#include "IGameServerConnection.h"
#include "MessageHeader.h"
#include "Networking.h"
#include "PeerInfo.h"
#include "UdpNetworkPlayer.h"
#include "events/EventSubscriber.h"
#include "file/SerializationManager.h"

namespace gamelib
{
    class SerializationManager;
    class EventManager;
    class EventFactory;

    class LocalSocketGameServerConnection: public IGameServerConnection, public EventSubscriber
    {
    public:
        LocalSocketGameServerConnection(const char* socketPath, Encoding wireFormat);

        void Initialize() override;

        void Create() override;

        void CheckForPlayerTraffic(unsigned long deltaMs) override;

        static void RaiseNetworkTrafficReceivedEvent(const char *buffer, int bytesReceived, const PeerInfo &fromClient);

        static void ProcessRequestPlayerDetailsMessage(const MessageHeader &messageHeader, const PeerInfo &fromClient);

        void Listen(unsigned long deltaMs) override;

        static void ProcessPingMessage(PeerInfo fromClient);

        static void ParseReceivedPlayerPayload(const char *inPayload, int payloadLength, const PeerInfo &fromClient);

        static void SendToConnectedPlayersExceptToSender(const std::string &senderNickname,
                                                         const std::string &serializedMessage);

        static int InternalSend(SOCKET socket, const char *buf, int len, int flags, const sockaddr *to, int toLen);

        void SendEventToAllPlayers(std::string serializedEvent) override;

        void Disconnect() override;

        std::string GetAddress() override;

        std::string GetPort() override;

        std::string GetSubscriberName() override;

        std::vector<std::shared_ptr<Event>>
        HandleEvent(const std::shared_ptr<Event> &evt, unsigned long deltaMs) override;
    private:
        SOCKET listeningSocket {};
        EventManager* eventManager {};
        Networking* networking {};
        EventFactory* eventFactory {};
        std::shared_ptr<SerializationManager> serializationManager {};
        const char *socketPath;
        timeval timeout{};
        fd_set readfds{};
        constexpr static auto ReadBufferMaxElements = 1024;
        char readBuffer[ReadBufferMaxElements]{};
        std::vector<UdpNetworkPlayer> players;
        Encoding TheEncoding;
    };
}

#endif //CPPGAMELIB_LOCALSOCKETSERVERCONNECTION_H