
#pragma once
#ifndef SERVER_WORLD_OPCODE_H
#define SERVER_WORLD_OPCODE_H


enum Opcode : uint16_t {
    STEST = 0,
    CTEST = 1,
    MAX = 2
};
constexpr size_t NUM_OPCODES = Opcode::MAX;

namespace OpcodeUtils {

constexpr char* ToString(uint16_t opcode) {
    switch (opcode) {
        case Opcode::STEST: return "STEST";
        case Opcode::CTEST: return "CTEST";
        default: return "MAX";
    }
}

constexpr bool IsClient(uint16_t opcode) {
    switch (opcode) {
        case Opcode::CTEST: return true;
        default: return false;
    }
}

};

class WorldSession;
class WorldPacket;

namespace HANDLERS {

#define DECLARE(name) bool name##(WorldSession* session, WorldPacket& packet)

DECLARE(Test);

#undef DECLARE

}; // namespace HANDLERS


#define HANDLER_REF(name) &HANDLERS::##name
#define NOOP_REF nullptr


typedef bool (*OpcodeHandler)(WorldSession*, WorldPacket&);
static const OpcodeHandler OPCODE_TABLE[NUM_OPCODES] = {
    /* STEST(0) = */ NOOP_REF,
    /* CTEST(1) = */ HANDLER_REF(Test),
    // we don't store MAX because we check against it when validating the packet
};

#undef HANDLER_REF
#undef NOOP_REF


#endif // SERVER_WORLD_OPCODE_H