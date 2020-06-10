#pragma once

#include "network/socket_handler.h"
#include "game/system/system_base.h"
#include "game/component/session.h"
#include "game/packet.h"
#include <entt/entt.hpp>
#include <moodycamel/concurrentqueue.h>
#include <cstdint>
#include <cstddef>
#include <memory>

namespace game {

namespace system {

class session : public system_base, public network::socket_handler {
public:
    template<typename T>
    using queue = moodycamel::ConcurrentQueue<T>;

    typedef struct message {
        uint32_t id;
        std::vector<uint8_t> data;
    } message;

    session(std::shared_ptr<packet::client::handler_table> handler_table = std::make_shared<packet::client::default_handler_table>());

    void setup(world& world) override;
    void update(world& world) override;

    void on_open(uint32_t id, std::weak_ptr<network::socket_base> socket) override;
    void on_close(uint32_t id) override;
    void on_message(uint32_t id, std::vector<uint8_t>&& data) override;
    void on_error(uint32_t id, std::string_view what, beast::error_code error) override;
private:
    void handle_connections(world& world);
    void handle_messages(world& world);
    void handle_disconnections(world& world);

    std::atomic<size_t> connection_counter_;
    queue<component::session> connection_queue_;

    std::atomic<size_t> message_counter_;
    queue<message> message_queue_;

    std::atomic<size_t> disconnection_counter_;
    queue<uint32_t> disconnection_queue_;

    std::shared_ptr<packet::client::handler_table> handler_table_;
}; // class session


} // namespace system
} // namespace game
