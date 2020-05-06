
#pragma once
#ifndef SERVER_WORLD_WORLD_H
#define SERVER_WORLD_WORLD_H

#include "pch.h"
#include "world/network/WorldSocketManager.h"
#include "world/network/WorldSession.h"
#include "world/util/WorldTime.h"

class Websocket;

class World {
public:
    static World& Instance();
    WorldSocketManager* GetSocketManager();

    void StartMainLoop(std::atomic<bool>* exitSignal, float updateRate = 30, int maxConsecutiveUpdates = 5);

    WorldTime::TimePoint GetTimePoint();
    uint32_t GetTimeStamp();
    std::string GetDate();
private:
    World();
    ~World();

    void Update();

    WorldSocketManager socketManager_;
    
    unsigned loopCount_;
    WorldTime::TimePoint startTime_;
    WorldTime::TimePoint currentTime_;
    
    std::mutex sessionsLock_;
    uint16_t sessionIdSequence_;
    std::unordered_map<uint16_t, std::shared_ptr<WorldSession>> sessions_;
}; // class World

#define sWorld World::Instance()


#endif // SERVER_WORLD_WORLD_H