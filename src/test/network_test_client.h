#pragma once

#include "network/boost.h"
#include "network/socket_handler.h"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <functional>
#include <string>
#include <string_view>
#include <atomic>

namespace network_test {

class client;

class client_handler {
public:
    virtual ~client_handler() = default;

    // Called when a socket is fully opened
    virtual void on_open() = 0;

    // Called when a socket is fully closed
    virtual void on_close() = 0;

    // Called when a message is received from a socket
    virtual void on_message(std::vector<uint8_t>&& data) = 0;

    // Called when the socket encounters an error
    // Sockets that got an error shouldn't be considered close until you receive an `on_close` event from it
    virtual void on_error(std::string_view what, beast::error_code error) = 0;
};

class client : public std::enable_shared_from_this<client>
{
public:
    client(
        net::io_context& ioc,
        std::string_view host,
        std::string_view port,
        std::shared_ptr<client_handler> chandler
    )   : host_(host)
        , port_(port)
        , ws_(net::make_strand(ioc))
        , fbuffer_()
        , client_handler_(chandler)
        , writing_(false)
        , wbuffer_()
    {}

    ~client() 
    {
        client_handler_->on_close();
    }

    void open() 
    {
        tcp::resolver resolver(ws_.get_executor());

        resolver.async_resolve(
            host_,
            port_,
            beast::bind_front_handler(
                &client::on_resolve,
                shared_from_this()));
    }

    void send(std::vector<uint8_t> data) 
    {
        net::post(
            ws_.get_executor(),
            beast::bind_front_handler(
                &client::on_send,
                shared_from_this(),
                std::move(data)));
    }

    void close() 
    {
        ws_.async_close(
            beast::websocket::close_code::normal,
            beast::bind_front_handler(
                &client::on_close,
                shared_from_this()));
    }

private:
    void on_resolve(beast::error_code ec, tcp::resolver::results_type results) 
    {
        if (ec) return client_handler_->on_error("on_resolve", ec);

        beast::get_lowest_layer(ws_).expires_after(std::chrono::seconds(10));

        beast::get_lowest_layer(ws_).async_connect(
            results,
            beast::bind_front_handler(
                &client::on_connect,
                shared_from_this()));
    }

    void on_connect(beast::error_code ec, tcp::resolver::results_type::endpoint_type) 
    {
        if (ec) return client_handler_->on_error("on_connect", ec);

        beast::get_lowest_layer(ws_).expires_never();

        ws_.set_option(
            beast::websocket::stream_base::timeout::suggested(
                beast::role_type::client));

        ws_.set_option(beast::websocket::stream_base::decorator(
            [](beast::websocket::request_type& req)
        {
            req.set(http::field::user_agent,
                std::string("TEST_CLIENT"));
        }));

        ws_.binary(true);

        ws_.async_handshake(host_, "/",
            beast::bind_front_handler(
                &client::on_handshake,
                shared_from_this()));
    }

    void on_handshake(beast::error_code ec) 
    {
        if (ec) return client_handler_->on_error("on_handshake", ec);

        client_handler_->on_open();

        ws_.async_read(
            fbuffer_,
            beast::bind_front_handler(
                &client::on_read,
                shared_from_this()));
    }

    void on_read(beast::error_code ec, std::size_t read_bytes) 
    {
        if (ec) return client_handler_->on_error("on_read", ec);

        {
            std::vector<uint8_t> buf(read_bytes);
            boost::asio::buffer_copy(
                boost::asio::buffer(buf.data(), read_bytes),
                fbuffer_.data());
            client_handler_->on_message(std::move(buf));
        }

        fbuffer_.consume(read_bytes);

        ws_.async_read(
            fbuffer_,
            beast::bind_front_handler(
                &client::on_read,
                shared_from_this()));
    }

    void on_send(std::vector<uint8_t>&& data) 
    {
        wbuffer_.push_back(std::move(data));

        if (writing_)
            return;

        writing_ = true;

        ws_.async_write(
            net::buffer(wbuffer_.front()),
            beast::bind_front_handler(
                &client::on_write,
                shared_from_this()));
    }

    void on_write(beast::error_code ec, std::size_t written_bytes) 
    {
        if (ec) return client_handler_->on_error("on_write", ec);

        wbuffer_.erase(wbuffer_.begin());

        if (wbuffer_.empty()) {
            writing_ = false;
            return;
        }

        ws_.async_write(
            net::buffer(wbuffer_.front()),
            beast::bind_front_handler(
                &client::on_write,
                shared_from_this()));
    }

    void on_close(beast::error_code ec) 
    {
        if (ec) return client_handler_->on_error("on_write", ec);
    }

    std::string host_;
    std::string port_;
    beast::websocket::stream<beast::tcp_stream> ws_;
    beast::flat_buffer fbuffer_;
    std::shared_ptr<client_handler> client_handler_;
    bool writing_;
    std::vector<std::vector<uint8_t>> wbuffer_;
};

} // namespace test