
#include "pch.h"
#include "World.h"
#include "core/network/Websocket.h"
#include "network/WorldPacket.h"
#include "opcode/Opcode.h"

World::World()
    : socketManager_()
    , loopCount_(0)
    , startTime_()
    , currentTime_()
    , sessionsLock_()
    , sessionIdSequence_(0)
    , sessions_()
{
    socketManager_.SetOnSocketAdd([&](Websocket* socket) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.insert(std::make_pair(
            sessionIdSequence_, 
            std::make_shared<WorldSession>(sessionIdSequence_, socket)
        ));
        sessionIdSequence_++;
    });

    socketManager_.SetOnSocketRemove([&](Websocket* socket) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            if(it->second->CheckIdentity(socket)) {
                sessions_.erase(it);
                break;
            }
        }
    });
}

World::~World()
{
}

World& World::Instance()
{
    static World instance;
    return instance;
}

WorldSocketManager* World::GetSocketManager()
{
    return &socketManager_;
}

void World::StartMainLoop(std::atomic<bool>* exitSignal, float updateRate, int maxConsecutiveUpdates)
{
    startTime_ = WorldTime::Now();
    currentTime_ = startTime_;

    auto shouldExit = [&exitSignal]()->bool { 
        return exitSignal->load(std::memory_order_acquire); 
    };

    WorldTime::Duration updateTimeDelta(1.f / updateRate);
    auto consecutiveUpdates = 0;

	while (!shouldExit()) {
        while(WorldTime::Now() > currentTime_ && consecutiveUpdates < maxConsecutiveUpdates) {
			this->Update();

            currentTime_ += updateTimeDelta;
        }
	}
}

WorldTime::TimePoint World::GetTimePoint()
{
    return currentTime_;
}

uint32_t World::GetTimeStamp()
{
    return static_cast<uint32_t>(
        std::lround(
            currentTime_.time_since_epoch().count() - startTime_.time_since_epoch().count()
        )
    );
}

std::string World::GetDate()
{
    return WorldTime::ToString(currentTime_);
}

void World::Update()
{
    {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            it->second->Update();
        }
    }
}