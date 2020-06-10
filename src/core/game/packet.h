#pragma once

#include <cstdint>
#include <type_traits>
#include <functional>
#include <entt/entt.hpp>
#include <vector>
#include "game/schema/packet_generated.h"
#include "game/world.h"

namespace game {

inline std::vector<uint8_t> to_vec(const flatbuffers::FlatBufferBuilder& fbs) {
    return std::vector<uint8_t>{fbs.GetBufferPointer(), fbs.GetBufferPointer() + fbs.GetSize()};
}

inline std::vector<uint8_t> to_vec(const flatbuffers::Vector<uint8_t>* data) {
    return std::vector<uint8_t>{data->begin(), data->begin() + data->size()};
}

namespace packet {

namespace client {

enum opcode : uint16_t {
	MOVE_INPUT = 0,
    MAX
};

struct handler {
    using function_type = std::function<void(world&, const entt::entity&, const std::vector<uint8_t>&)>;

    handler(uint16_t opcode, function_type fn);

    const uint16_t opcode;
    const function_type fn;
    void operator()(world& world, const entt::entity& entity, const std::vector<uint8_t>& packet);
};

class handler_table {
public:
    virtual ~handler_table() = default;
    virtual handler const& get(uint16_t opcode) = 0;
};

class default_handler_table : public handler_table {
public:
    handler const& get(uint16_t opcode) override;
};

constexpr std::string_view to_string(uint16_t opcode) {
#define CASE(op) case game::packet::client::opcode::op: return #op;
    switch (opcode) {
        CASE(MOVE_INPUT);
        default: return "NULL";
    }
#undef CASE
}

} // namespace client

namespace server {

enum opcode : uint16_t {
    ID = 0,
	STATE,
    MAX
};

constexpr std::string_view to_string(uint16_t opcode) {
#define CASE(op) case game::packet::server::opcode::op: return #op;
    switch (opcode) {
        CASE(ID);
        CASE(STATE);
        default: return "NULL";
    }
#undef CASE
}

} // namespace server

inline std::vector<uint8_t> build(server::opcode opcode, const std::vector<uint8_t>& buffer) {
    flatbuffers::FlatBufferBuilder builder{ static_cast<flatbuffers::uoffset_t>(2) + buffer.size() };
    auto pkt = CreatePacket(builder, opcode, builder.CreateVector(buffer.data(), buffer.size()));
    builder.Finish(pkt);
    return to_vec(builder);
}

} // namespace packet

} // namespace game