
#include "World.h"
#include "network/Websocket.h"

World::World()
    : socketManager_()
    , loopCount_(0)
    , worldTime_(WorldTime::Now())
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
    });

    socketManager_.SetOnSocketRemove([&](Websocket* socket) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            if(it->second->CompareSocketPtr(socket)) {
                sessions_.erase(it);
                break;
            }
        }
    });

    spdlog::info("World initialized");
}

World::~World()
{
}

World* World::Instance()
{
    static World instance;
    return &instance;
}

WorldSocketManager* World::GetSocketManager()
{
    return &socketManager_;
}

void World::StartMainLoop(std::atomic<bool>* exitSignal, float updateRate, int maxConsecutiveUpdates)
{
    auto shouldExit = [&exitSignal]()->bool { 
        return exitSignal->load(std::memory_order_acquire); 
    };

    WorldTime::Duration updateTimeDelta(1000.f / updateRate);
    auto consecutiveUpdates = 0;

	while (!shouldExit()) {
        while(WorldTime::Now() > worldTime_ && consecutiveUpdates < maxConsecutiveUpdates) {
			this->Update();

            worldTime_ += updateTimeDelta;
        }
	}
}

WorldTime::TimePoint World::GetWorldTime()
{
    return worldTime_;
}

std::string World::GetWorldDate()
{
    return WorldTime::ToString(worldTime_);
}

void World::Update()
{
    { // update sessions
        std::lock_guard<std::mutex> lock(sessionsLock_);
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            it->second->Update();
        }
    }
}