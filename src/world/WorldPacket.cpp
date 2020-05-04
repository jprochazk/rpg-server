
#include "pch.h"
#include "WorldPacket.h"
#include "World.h"
#include "Opcode.h"

WorldPacket::WorldPacket(ByteBuffer&& buffer, uint32_t time)
    : ByteBuffer(std::move(buffer))
    , opcode_(Opcode::MAX)
    , time_(time)
{
    std::optional<uint16_t> opt_opcode;
    *this >> opt_opcode;
    if(opt_opcode && OpcodeUtils::IsClient(opt_opcode.value()))  {
        opcode_ = opt_opcode.value();
    }
}
WorldPacket::WorldPacket(uint16_t opcode, uint32_t time)
    : ByteBuffer()
    , opcode_(opcode)
    , time_(time)
{
    *this << opcode;
    *this << time;
}

bool WorldPacket::IsValid() const noexcept
{
    return opcode_ < Opcode::MAX && Size() < MAX_PACKET_SIZE;
}

uint16_t WorldPacket::GetOpcode() const noexcept
{
    if(IsValid()) return opcode_;
    return Opcode::MAX;
}

uint32_t WorldPacket::GetTime() const noexcept
{
    return time_;
}