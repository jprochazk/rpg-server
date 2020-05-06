
#include "pch.h"
#include "SocketListener.h"
#include "Websocket.h"
#include "SocketManager.h"

SocketListener::SocketListener(
    net::io_context& ioc,
    tcp::endpoint endpoint,
    SocketManager* socket_manager)   
    : ioc_(ioc)
    , acceptor_(ioc)
    , socketManager_(socket_manager)
{

    beast::error_code ec;

    // Open the acceptor
    acceptor_.open(endpoint.protocol(), ec);
    if (ec)
    {
        Fail(ec, "open");
        return;
    }

    // Allow address reuse
    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
    {
        Fail(ec, "set_option");
        return;
    }

    // Bind to the server address
    acceptor_.bind(endpoint, ec);
    if (ec)
    {
        Fail(ec, "bind");
        return;
    }

    // Start listening for connections
    acceptor_.listen(
        net::socket_base::max_listen_connections, ec);
    if (ec)
    {
        Fail(ec, "listen");
        return;
    }
}

SocketListener::~SocketListener()
{
}

void SocketListener::Run()
{
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &SocketListener::OnAccept,
            shared_from_this()));
}

// Report a failure
void SocketListener::Fail(beast::error_code ec, char const* what)
{
    // Don't report on canceled operations
    if (ec == net::error::operation_aborted)
        return;
    LOG_ERROR("{}: {}", what, ec.message());
}

// Handle a connection
void SocketListener::OnAccept(beast::error_code ec, tcp::socket socket)
{
    if (ec) {
        return Fail(ec, "accept");
    } else {
        // Launch a new session for this connection
        std::make_shared<Websocket>(
            std::move(socket),
            socketManager_
        )->Run();
    }
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        beast::bind_front_handler(
            &SocketListener::OnAccept,
            shared_from_this()));
}
