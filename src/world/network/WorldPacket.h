#pragma once
#ifndef SERVER_WORLD_WORLD_PACKET_H
#define SERVER_WORLD_WORLD_PACKET_H

#include "pch.h"
#include "core/network/ByteBuffer.h"
#include "world/util/WorldTime.h"
#include "world/opcode/Opcode.h"

class WorldPacket : public ByteBuffer {
public:
    WorldPacket(ByteBuffer&& buffer, uint32_t time);
    WorldPacket(uint16_t opcode, uint32_t time);
    ~WorldPacket() = default;

    static constexpr size_t MAX_PACKET_SIZE = 1024;

    bool IsValid() const noexcept;
    uint16_t GetOpcode() const noexcept;
    uint32_t GetTime() const noexcept;
private:
    uint16_t opcode_;
    uint32_t time_;
};

#endif // SERVER_WORLD_WORLD_PACKET_H