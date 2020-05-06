
#include "pch.h"
#include "WorldSocketManager.h"
#include "core/network/Websocket.h"

WorldSocketManager::WorldSocketManager()
    : SocketManager()
    , socketAddCallback_(nullptr)
    , socketRemoveCallback_(nullptr)
{
}

WorldSocketManager::~WorldSocketManager()
{
}

void WorldSocketManager::SetOnSocketAdd(std::function<void(Websocket*)> callback)
{
    socketAddCallback_ = callback;
}

void WorldSocketManager::SetOnSocketRemove(std::function<void(Websocket*)> callback)
{
    socketRemoveCallback_ = callback;
}

void WorldSocketManager::Add(Websocket* session)
{
    if(socketAddCallback_)
        socketAddCallback_(session);
}

void WorldSocketManager::Remove(Websocket* session)
{
    if(socketRemoveCallback_)
        socketRemoveCallback_(session);
}