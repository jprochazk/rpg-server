
#include "pch.h"
#include "world/Opcode.h"
#include "world/WorldSession.h"
#include "world/WorldPacket.h"

const char* OpcodeToString(uint16_t opcode) {
    switch(opcode) {
        case Opcode::STEST: return "STEST";
        case Opcode::CTEST: return "CTEST";
        default: return "MAX";
    }
}

bool IsClientOpcode(uint16_t opcode) {
    switch(opcode) {
        case Opcode::CTEST: return true;
        default: return false;
    }
}

// TODO debug this

bool HandleOpcode_Test(WorldSession* session, WorldPacket& packet) {
    std::optional<uint32_t> sequence;
    packet >> sequence;

    if(!sequence) {
        return false;
    }

    spdlog::info("Opcode {} from session id {}. Data: {{ sequence: {} }}", 
        OpcodeToString(packet.GetOpcode()), 
        session->GetId(), 
        sequence.value()
    );

    spdlog::info("Closing session id {}", session->GetId());
    session->Close();

    return true;
}