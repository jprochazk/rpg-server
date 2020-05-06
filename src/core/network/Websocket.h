
#pragma once
#ifndef SERVER_CORE_NETWORK_WEBSOCKET_H
#define SERVER_CORE_NETWORK_WEBSOCKET_H

#include "pch.h"
#include "core/network/net.h"


const uint16_t MAX_PACKET_SIZE = 1024;

class SocketManager;
class PacketHandler;

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
    void Send(std::vector<uint8_t> ss);
    void SetPacketHandler(PacketHandler* handler);
private:
    beast::flat_buffer wsBuffer_;
    beast::websocket::stream<beast::tcp_stream> ws_;

    bool isAsyncWriting_;
    std::vector<std::vector<uint8_t>> writeBuffer_;

    SocketManager* socketManager_;
    PacketHandler* packetHandler_;

    void Fail(beast::error_code ec, char const* what);
    void OnAccept(beast::error_code ec);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnSend(std::vector<uint8_t>&& ss);
    void OnClose();

    void AsyncClose();
    void OnAsyncClose(beast::error_code ec);
}; // class Websocket

#endif // SERVER_CORE_NETWORK_WEBSOCKET_H
