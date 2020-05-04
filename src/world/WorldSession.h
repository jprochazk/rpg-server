
#pragma once
#ifndef SERVER_WORLD_WORLD_SESSION_H
#define SERVER_WORLD_WORLD_SESSION_H

#include "pch.h"
#include "core/common/ByteBuffer.h"
#include "world/WorldPacket.h"

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

    /// Close the session
    void Close();

    /// Send a single packet to the session
    void SendPacket(WorldPacket packet);

    /// Used to identify a session by the underlying Websocket pointer address
    bool CheckIdentity(Websocket* that) const noexcept;

    uint16_t GetId() const noexcept;
private:
    uint16_t id_;
    Websocket* socket_;
};

#endif // SERVER_WORLD_WORLD_SESSION_H
