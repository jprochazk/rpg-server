
#pragma once
#ifndef SERVER_NETWORK_WEBSOCKET_H
#define SERVER_NETWORK_WEBSOCKET_H

#include "pch.h"
#include "common/ByteBuffer.h"
#include "network/net.h"

class SocketManager;
class Websocket;

const uint16_t MAX_PACKET_SIZE = 1024;

/** Represents an active WebSocket connection to the server
*/
class Websocket : public std::enable_shared_from_this<Websocket>
{
public:
    Websocket(
        tcp::socket&& socket,
        SocketManager* socketManager);

    ~Websocket();

    void Run();
    void Close();
    void Send(const ByteBuffer ss);
    bool IsBufferEmpty() const;
    std::vector<ByteBuffer> GetBuffer();
private:
    beast::flat_buffer wsBuffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;

    bool shouldClose_;

    bool isAsyncWriting_;
    std::vector<ByteBuffer> writeBuffer_;

    std::mutex readLock_;
    std::vector<ByteBuffer> readBuffer_;
    SocketManager* socketManager_;


    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnSend(const ByteBuffer ss);
    void OnClose();

    void AsyncClose();
    void OnAsyncClose(beast::error_code ec);
}; // class Websocket

#endif // SERVER_NETWORK_WEBSOCKET_H
