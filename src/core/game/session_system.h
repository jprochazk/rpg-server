#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include "network/socket_handler.h"
#include "game/system.h"
#include "game/session_component.h"
#include <entt/entt.hpp>
#include <moodycamel/concurrentqueue.h>

namespace game {

class session_system : public system, public network::socket_handler {
public:
    template<typename T>
    using queue = moodycamel::ConcurrentQueue<T>;

    typedef struct message {
        uint32_t id;
        std::vector<uint8_t> data;
    } message;

    virtual void update(entt::registry& registry) override;

    virtual void on_open(uint32_t id, std::weak_ptr<network::socket> socket) override;
    virtual void on_close(uint32_t id) override;
    virtual void on_message(uint32_t id, std::vector<uint8_t>&& data) override;
    virtual void on_error(uint32_t id, std::string_view what, beast::error_code error) override;
private:
    void handle_connections(entt::registry& registry);
    void handle_messages(entt::registry& registry);
    void handle_disconnections(entt::registry& registry);

    std::atomic<size_t> connection_counter_;
    queue<session> connection_queue_;

    std::atomic<size_t> message_counter_;
    queue<message> message_queue_;

    std::atomic<size_t> disconnection_counter_;
    queue<uint32_t> disconnection_queue_;
}; // class session_system

} // namespace game
