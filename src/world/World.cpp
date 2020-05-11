
#include "pch.h"
#include "World.h"
#include "core/network/Websocket.h"
#include "network/packet/WorldPacket.h"
#include "opcode/Opcode.h"
#include "core/network/stats/NetworkStats.h"

World::World()
    : socketManager_()
    , loopCount_(0)
    , startTime_()
    , currentTime_()
    , sessionsLock_()
    , sessionIdSequence_()
    , sessions_()
    , lastNetworkMeasure_(std::chrono::steady_clock::now())
{
    socketManager_.SetOnSocketAdd([&](Websocket* socket) {
        auto id = sessionIdSequence_.Get();
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.insert(std::make_pair(
            id,
            std::make_shared<WorldSession>(id, socket)
        ));
    });

    socketManager_.SetOnSocketRemove([&](Websocket* socket) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            auto session = it->second;
            if(session->CheckIdentity(socket)) {
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
            auto session = it->second;
            session->Update();
        }

        auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            now - lastNetworkMeasure_).count() >= 1000) {
            lastNetworkMeasure_ = now;

            auto measurement = NetworkStats::Measure();
            spdlog::info(measurement.ToString());
        }
    }
}