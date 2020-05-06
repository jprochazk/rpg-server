
#pragma once
#ifndef SERVER_CORE_NETWORK_SOCKET_LISTENER_H
#define SERVER_CORE_NETWORK_SOCKET_LISTENER_H

#include "pch.h"

#include "core/network/net.h"
#include "core/network/Websocket.h"

// Accepts incoming connections and launches the sessions
class SocketListener : public std::enable_shared_from_this<SocketListener>
{
public: // public interface
    SocketListener(
        net::io_context& ioc,
        tcp::endpoint endpoint,
        SocketManager* socket_manager);
    ~SocketListener();

    void Run();
private: // internal
    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec, tcp::socket socket);
private: // members
    net::io_context& ioc_;
    tcp::acceptor acceptor_;
    SocketManager* socketManager_;
}; // class SocketListener

#endif // SERVER_CORE_NETWORK_SOCKET_LISTENER_H
