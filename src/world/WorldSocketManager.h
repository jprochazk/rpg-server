
#pragma once
#ifndef SERVER_WORLD_WORLD_SOCKET_MANAGER_H
#define SERVER_WORLD_WORLD_SOCKET_MANAGER_H

#include "pch.h"
#include "network/SocketManager.h"

class Websocket;

class WorldSocketManager : public SocketManager {
public:
    WorldSocketManager();
    ~WorldSocketManager();
    
    void Add(Websocket* session) override;
    void Remove(Websocket* session) override;

    void SetOnSocketAdd(std::function<void(Websocket*)> callback);
    void SetOnSocketRemove(std::function<void(Websocket*)> callback);
private:
    std::function<void(Websocket*)> socketAddCallback_;
    std::function<void(Websocket*)> socketRemoveCallback_;
}; // class WorldSocketManager


#endif // SERVER_WORLD_WORLD_SOCKET_MANAGER_H