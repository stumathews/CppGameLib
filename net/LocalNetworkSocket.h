//
// Created by stuart on 11/01/2026.
//

#ifndef CPPGAMELIB_LOCALUNIXSOCKET_H
#define CPPGAMELIB_LOCALUNIXSOCKET_H
#include "IGameClientConnection.h"

// A Unix Domain Socket (local only)
class LocalNetworkSocket final : public gamelib::IGameClientConnection // As this is a prototype, we'll use the IConnectedNetworkSocket
{
public:
    explicit LocalNetworkSocket();

    bool IsTcp() override;
    int Send(const char* data, int dataLength, unsigned long deltaMs) override;
    int Receive(char* readBuffer, int bufLength, unsigned long deltaMs) override;
    bool IsValid() override;
    [[nodiscard]] SOCKET GetRawSocket() const override;
    ~LocalNetworkSocket() override = default;

    // IConnectedNetworkSocket that requires a port which we'll not use
    void Connect(const char* socketPath, const char* unused) override;

private:
    SOCKET socket = -1;
    const char* socketPath;
};


#endif //CPPGAMELIB_LOCALUNIXSOCKET_H