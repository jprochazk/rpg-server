
#include "game/system/session.h"
#include "game/component/position.h"
#include "game/component/velocity.h"
#include "common/assert.h"
#include "game/schema/id_generated.h"
#include "game/schema/packet_generated.h"
#include <map>
#include <set>
#include <spdlog/spdlog.h>
#include <iostream>

namespace game {
namespace system {

session::session(std::shared_ptr<packet::client::handler_table> handler_table)
    : handler_table_(std::move(handler_table))
    , connection_counter_(0)
    , message_counter_(0)
    , disconnection_counter_(0)
{
}

void session::setup(world& world) {
}

void session::update(world& world) {
    handle_connections(world);
    handle_messages(world);
    handle_disconnections(world);
}

void session::on_open(uint32_t id, std::weak_ptr<network::socket_base> socket) {
    connection_queue_.enqueue(component::session{ id,std::move(socket) });
    connection_counter_.fetch_add(1, std::memory_order_release);
}

void session::on_close(uint32_t id) {
    disconnection_queue_.enqueue(id);
    disconnection_counter_.fetch_add(1, std::memory_order_release);
}

void session::on_message(uint32_t id, std::vector<uint8_t>&& data) {
    message_queue_.enqueue(message{ id,std::move(data) });
    message_counter_.fetch_add(1, std::memory_order_release);
}

void session::on_error(uint32_t id, std::string_view what, beast::error_code error) {
    if (error == net::error::operation_aborted ||
        error == net::error::connection_aborted ||
        error == beast::websocket::error::closed)
        return;

    spdlog::error("Socket ID {} -> [{}] ERROR: {}", id, what, error.message());
}

void session::handle_connections(world& world) {
    //spdlog::info("nc {}", connection_counter_.load(std::memory_order_acquire));
    auto num_connections = connection_counter_.load(std::memory_order_acquire);
    //spdlog::info("nc {}", num_connections);
    connection_counter_.fetch_add(-static_cast<int64_t>(num_connections), std::memory_order_release);

    if (num_connections == 0)
        return;

    //spdlog::info("nc {}", num_connections);

    std::vector<component::session> sessions(num_connections, component::session{});
    size_t received = connection_queue_.try_dequeue_bulk(sessions.begin(), num_connections);
    debug_assert(received == num_connections, 
        "received != num_connections: {} != {}", received, num_connections);

    auto& registry = world.get_registry();
    std::vector<entt::entity> entities(num_connections);
    registry.create(entities.begin(), entities.end());

    debug_assert(sessions.size() == entities.size(),
        "sessions.size != entities.size: {} != {}", sessions.size(), entities.size());
    // sessions.size() == entities.size() - it's safe to iterate both at once

    for (size_t i = 0; i < num_connections; i++) {
        // only add sessions for which we can acquire a socket
        std::shared_ptr<network::socket_base> sock = sessions[i].socket.lock();
        if (!sock) return;

        spdlog::info("Session ID {} joined", sessions[i].id);
        registry.emplace<component::session>(entities[i], std::move(sessions[i]));
        registry.emplace<component::position>(entities[i], component::position{ 0,0 });
        registry.emplace<component::velocity>(entities[i], component::velocity{ 0,0 });

        flatbuffers::FlatBufferBuilder builder{ 16 };
        auto id = CreateId(builder, static_cast<uint32_t>(entities[i]));
        builder.Finish(id);

        sock->send(packet::build(packet::server::opcode::ID, game::to_vec(builder)));
    }
}

void session::handle_messages(world& world) {
    auto num_messages = message_counter_.load(std::memory_order_acquire);
    message_counter_.fetch_add(-static_cast<int64_t>(num_messages), std::memory_order_release);

    if (num_messages == 0)
        return;

    std::vector<message> vmessages(num_messages, message{});
    message_queue_.try_dequeue_bulk(vmessages.begin(), num_messages);

    // turn vector into multimap for efficient lookup
    std::multimap<uint32_t, std::vector<uint8_t>> mmessages;
    for (auto& m : vmessages) {
        mmessages.insert(std::make_pair(m.id, std::move(m.data)));
    }
    vmessages.erase(vmessages.begin(), vmessages.end());

    auto& registry = world.get_registry();
    registry.view<component::session>().each(
    [&](const entt::entity& entity, component::session& session) {
        const auto [start, end] = mmessages.equal_range(session.id);
        for (auto it = start; it != end; it++) {
            auto& [id, buffer] = *it;

            bool ok = VerifyPacketBuffer(flatbuffers::Verifier(buffer.data(), buffer.size()));
            if (!ok) continue;

            auto packet = GetPacket(buffer.data());

            auto handler = handler_table_->get(packet->opcode());
            handler(world, entity, to_vec(packet->buffer()));
        }
    });
}

void session::handle_disconnections(world& world) {
    auto num_disconnections = disconnection_counter_.load(std::memory_order_acquire);
    disconnection_counter_.fetch_add(-static_cast<int64_t>(num_disconnections), std::memory_order_release);

    if (num_disconnections == 0)
        return;

    std::vector<uint32_t> vdisconnections(num_disconnections, 0);
    disconnection_queue_.try_dequeue_bulk(vdisconnections.begin(), num_disconnections);

    std::set<uint32_t> sdisconnections;
    for (auto& dc : vdisconnections) {
        sdisconnections.insert(dc);
    }

    auto& registry = world.get_registry();
    // iterate over all entities with the session component,
    // removing any where disconnections.has(entity.session.id_sequence)
    registry.view<component::session>().each(
    [&](const entt::entity& e, component::session& s) {
        if (auto it = sdisconnections.find(s.id); it != sdisconnections.end()) {
            spdlog::info("Session ID {} left", s.id);
            registry.destroy(e);
        }
    });
}

} // namespace system
} // namespace game