
#include "pch.h"
#include "Opcode.h"
#include "WorldSession.h"
#include "WorldPacket.h"

namespace HANDLERS {

#define IMPL(name) bool name##(WorldSession* session, WorldPacket& packet)

IMPL(Test) {
    std::optional<uint32_t> sequence;
    packet >> sequence;

    if (!sequence) {
        return false;
    }

    spdlog::info("Opcode {} from session id {}. Data: {{ sequence: {} }}",
        OpcodeUtils::ToString(packet.GetOpcode()),
        session->GetId(),
        sequence.value()
    );

    spdlog::info("Closing session id {}", session->GetId());
    session->Close();

    return true;
}

#undef IMPL

}; // namespace HANDLERS
