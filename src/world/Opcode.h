
#pragma once
#ifndef SERVER_WORLD_OPCODE_H
#define SERVER_WORLD_OPCODE_H

enum Opcode : uint16_t {
    STEST = 0,
    CTEST = 1,
    MAX = 2
};
constexpr size_t NUM_OPCODES = Opcode::MAX;

const char* OpcodeToString(uint16_t opcode);
bool IsClientOpcode(uint16_t opcode);

class WorldSession;
class WorldPacket;

typedef bool (*OpcodeHandler)(WorldSession*,WorldPacket&);

bool HandleOpcode_Test(WorldSession* session, WorldPacket& packet);

static const OpcodeHandler OPCODE_TABLE[NUM_OPCODES] = {
    /* STEST(0) = */ nullptr,
    /* CTEST(1) = */ &HandleOpcode_Test,
    // we don't store MAX because we check against it when validating the packet
};


#endif // SERVER_WORLD_OPCODE_H