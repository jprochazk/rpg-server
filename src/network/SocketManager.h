
#pragma once
#ifndef SERVER_NETWORK_SOCKET_MANAGER_H

class Websocket;

class SocketManager {
public:
    virtual ~SocketManager() = default;
    virtual void Add(Websocket* session) = 0;
    virtual void Remove(Websocket* session) = 0;

}; // class SocketManager


#endif // SERVER_NETWORK_SOCKET_MANAGER_H