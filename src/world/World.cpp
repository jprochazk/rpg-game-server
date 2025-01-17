
#include "World.h"
#include "network/Websocket.h"
#include "world/WorldPacket.h"
#include "world/Opcode.h"

World::World()
    : socketManager_()
    , loopCount_(0)
    , startTime_()
    , worldTime_()
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
    startTime_ = WorldTime::Now();
    worldTime_ = startTime_;

    auto shouldExit = [&exitSignal]()->bool { 
        return exitSignal->load(std::memory_order_acquire); 
    };

    WorldTime::Duration updateTimeDelta(1.f / updateRate);
    auto consecutiveUpdates = 0;

	while (!shouldExit()) {
        while(WorldTime::Now() > worldTime_ && consecutiveUpdates < maxConsecutiveUpdates) {
			this->Update();

            worldTime_ += updateTimeDelta;
        }
	}
}

WorldTime::TimePoint World::GetTimePoint()
{
    return worldTime_;
}

uint32_t World::GetTimeStamp()
{
    return static_cast<uint32_t>(
        std::lround(
            worldTime_.time_since_epoch().count() - startTime_.time_since_epoch().count()
        )
    );
}

std::string World::GetDate()
{
    return WorldTime::ToString(worldTime_);
}

void World::Update()
{
    // TEMP make empty packet with just an opcode
    //WorldPacket pkt(Opcode::STEST, GetTimeStamp());
    
    { // update sessions
        std::lock_guard<std::mutex> lock(sessionsLock_);

        spdlog::info("Updating {} sessions", sessions_.size());
        for(auto it = sessions_.begin(); it != sessions_.end(); it++) {
            it->second->Update();

            // TEMP send a packet
            //it->second->SendPacket(pkt);
        }
    }
}