
#pragma once
#ifndef SERVER_WORLD_WORLD_TIME_H
#define SERVER_WORLD_WORLD_TIME_H

#include "pch.h"

class WorldTime {
private:
    WorldTime();
    virtual ~WorldTime() = 0;
public:
    using Clock = std::chrono::system_clock;
    using Duration = std::chrono::duration<double>;
    using TimePoint = std::chrono::time_point<Clock, Duration>;

    static TimePoint Now();
    static std::string ToString(TimePoint t);
}; // class WorldTime

#endif // SERVER_WORLD_WORLD_TIME_H
