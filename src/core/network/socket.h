#pragma once

#include "network/boost.h"
#include <memory>
#include <atomic>
#include <vector>
#include <cstdint>
#include <boost/core/noncopyable.hpp>

namespace network {

class socket_handler;

class socket : public std::enable_shared_from_this<socket>, private boost::noncopyable {
public:
    socket(uint32_t id, tcp::socket&& tcp_socket, std::shared_ptr<socket_handler> socket_handler);
    ~socket();

    void open();
    void close();
    void send(std::vector<uint8_t> data);
    bool closed() const;

private:
    void on_accept(beast::error_code ec);
    void on_close(beast::error_code ec);
    void on_read(beast::error_code ec, std::size_t rb);
    void on_send(std::vector<uint8_t>&& data);
    void on_write(beast::error_code ec, std::size_t wb);

private:
    uint32_t id_;
    beast::flat_buffer fbuffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;
    bool writing_;
    std::vector<std::vector<uint8_t>> wbuffer_;

    std::atomic<bool> closed_;
    std::shared_ptr<socket_handler> socket_handler_;
}; // class socket

} // namespace network

bool operator==(const network::socket& lhs, const network::socket& rhs);

