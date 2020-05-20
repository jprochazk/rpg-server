
#include "game/session_system.h"
#include <map>
#include <set>
#include "common/assert.h"
#include <spdlog/spdlog.h>

namespace game {

void session_system::update(entt::registry& registry) {
    handle_connections(registry);
    handle_messages(registry);
    handle_disconnections(registry);
}

void session_system::on_open(uint32_t id, std::weak_ptr<network::socket> socket) {
    spdlog::info("Socket ID {} -> opened", id);
    connection_queue_.enqueue(session{ id,std::move(socket) });
    connection_counter_.fetch_add(1, std::memory_order_release);
}

void session_system::on_close(uint32_t id) {
    spdlog::info("Socket ID {} -> closed", id);
    disconnection_queue_.enqueue(id);
    disconnection_counter_.fetch_add(1, std::memory_order_release);
}

void session_system::on_message(uint32_t id, std::vector<uint8_t>&& data) {
    spdlog::info("Socket ID {} -> message size {}", id, data.size());
    message_queue_.enqueue(message{ id,std::move(data) });
    message_counter_.fetch_add(1, std::memory_order_release);
}

void session_system::on_error(uint32_t id, std::string_view what, beast::error_code error) {
    if (error == net::error::operation_aborted ||
        error == net::error::connection_aborted ||
        error == beast::websocket::error::closed)
        return;

    spdlog::error("Socket ID {} -> [{}] ERROR: {}", id, what, error.message());
}



void session_system::handle_connections(entt::registry& registry) {
    auto num_connections = connection_counter_.load(std::memory_order_acquire);
    // we're not setting to 0, but just subtracting what we got,
    // because another thread may have incremented the counter inbetween these two calls
    connection_counter_.fetch_add(-static_cast<int64_t>(num_connections), std::memory_order_release);

    if (num_connections == 0)
        return;

    // default initialize a vector of size new_connections
    std::vector<session> sessions(num_connections, session{});
    size_t received = connection_queue_.try_dequeue_bulk(sessions.begin(), num_connections);
    DEBUG_ASSERT(received == num_connections, "received != num_connections: {} != {}", received, num_connections);

    // create new entities
    std::vector<entt::entity> entities(num_connections);
    registry.create(entities.begin(), entities.end());

    DEBUG_ASSERT(sessions.size() == entities.size(),
        "sessions.size != entities.size: {} != {}", sessions.size(), entities.size());
    // sessions.size() == entities.size() - it's safe to iterate both at once

    for (size_t i = 0; i < num_connections; i++) {
        // a session object is a 32-bit unsigned integer and a weak_ptr<network::socket>
        // both are cheap to copy, but seeing as both vectors that we created
        // will be dropped at the end of this function, we can freely
        // invalidate the vectors
        spdlog::info("New session ID {}", sessions[i].id);
        registry.emplace<session>(entities[i], std::move(sessions[i]));
    }
}

void session_system::handle_messages(entt::registry& registry) {
    auto num_messages = message_counter_.load(std::memory_order_acquire);
    message_counter_.fetch_add(-static_cast<int64_t>(num_messages), std::memory_order_release);

    if (num_messages == 0)
        return;

    std::vector<message> vmessages(num_messages, message{});
    message_queue_.try_dequeue_bulk(vmessages.begin(), num_messages);

    // turn vector into multimap for easy search later
    std::multimap<uint32_t, std::vector<uint8_t>> mmessages;
    for (auto& m : vmessages) {
        mmessages.insert(std::make_pair(m.id, std::move(m.data)));
    }
    // moving things out invalidates the vector, so clear it
    vmessages.erase(vmessages.begin(), vmessages.end());

    // iterate over all entities with session components
    registry.view<session>().each([&mmessages](const entt::entity& e, session& s) {
        const auto range = mmessages.equal_range(s.id);
        // i should be able to iterate over this
        for (auto it = range.first; it != range.second; it++) {
            auto& msg = it->second;
            // handle the message
            // TODO: packet handling
            spdlog::info("Received message size {} from session ID {}", msg.size(), s.id);
        }
    });
}

void session_system::handle_disconnections(entt::registry& registry) {
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

    // iterate over all entities with the session component,
    // removing any where disconnections.has(entity.session.id)
    registry.view<session>().each([&registry, &sdisconnections](const entt::entity& e, session& s) {
        if (auto it = sdisconnections.find(s.id); it != sdisconnections.end()) {
            spdlog::info("Closed session ID {}", s.id);
            registry.destroy(e);
        }
    });
}

} // namespace game