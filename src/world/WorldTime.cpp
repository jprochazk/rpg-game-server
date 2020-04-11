
#include "WorldTime.h"
#include <spdlog/spdlog.h>

WorldTime::WorldTime() {}
WorldTime::~WorldTime() {}

WorldTime::TimePoint WorldTime::Now() {
    return std::chrono::time_point<WorldTime::Clock, WorldTime::Duration>(
        std::chrono::duration_cast<WorldTime::Duration>(WorldTime::Clock::now().time_since_epoch())
    );
}

std::string WorldTime::ToString(WorldTime::TimePoint t)
{
    try {
        auto wct = std::chrono::time_point<WorldTime::Clock, WorldTime::Clock::duration>(
            std::chrono::duration_cast<WorldTime::Clock::duration>(t.time_since_epoch())
        );
        auto time = WorldTime::Clock::to_time_t(wct);
        return std::string(std::ctime(&time));
    } catch(...) {
        spdlog::error("failed to convert time to ctime");
        return {};
    }
}