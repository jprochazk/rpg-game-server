
#pragma once
#ifndef SERVER_WORLD_WORLD_SESSION_H
#define SERVER_WORLD_WORLD_SESSION_H

#include "common/SharedDefines.h"
#include "common/ByteBuffer.h"

class Websocket;
class World;

class WorldSession {
public:
    WorldSession(uint16_t id, Websocket* socket);
    ~WorldSession();
    WorldSession(const WorldSession&) = delete; // no copies
    WorldSession& operator=(const WorldSession&) = delete; // no self-assignments

    /// Update the session
    void Update();

    /// UNIMPLEMENTED
    void SendPacket(/*WorldPacket packet*/);

    /// Used to identify a session by the underlying Websocket pointer address
    bool CompareSocketPtr(Websocket* that);
private:
    uint16_t id_;
    Websocket* socket_;
};

#endif // SERVER_WORLD_WORLD_SESSION_H
