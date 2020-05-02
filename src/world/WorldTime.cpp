
#include "pch.h"
#include "WorldTime.h"

WorldTime::WorldTime() {}
WorldTime::~WorldTime() {}

WorldTime::TimePoint WorldTime::Now() {
    return std::chrono::time_point<WorldTime::Clock, WorldTime::Duration>(
        std::chrono::duration_cast<WorldTime::Duration>(WorldTime::Clock::now().time_since_epoch())
    );
}
std::string WorldTime::ToString(WorldTime::TimePoint t)
{
    auto time = WorldTime::Clock::to_time_t(
        std::chrono::time_point<WorldTime::Clock, WorldTime::Clock::duration>(
            std::chrono::duration_cast<WorldTime::Clock::duration>(t.time_since_epoch())));

    auto ctime = std::string(std::ctime(&time));
    return ctime.substr(0, ctime.length() - 1);
}