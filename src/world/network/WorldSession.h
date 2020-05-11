
#pragma once
#ifndef SERVER_WORLD_WORLD_SESSION_H
#define SERVER_WORLD_WORLD_SESSION_H

#include "pch.h"
#include "core/network/ByteBuffer.h"
#include "core/network/PacketHandler.h"

class Websocket;
class World;
class WorldPacket;
class GameObject;

class WorldSession : PacketHandler {
public:
    WorldSession(uint16_t id, Websocket* socket);
    virtual ~WorldSession() override;
    WorldSession(const WorldSession&) = delete; // no copies
    WorldSession& operator=(const WorldSession&) = delete; // no self-assignments

    virtual void Handle(std::vector<uint8_t>&& packet) override;

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

    std::mutex packetBufferMutex_;
    std::vector<WorldPacket> packetBuffer_;
};

#endif // SERVER_WORLD_WORLD_SESSION_H
