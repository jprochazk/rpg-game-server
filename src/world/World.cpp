
#include "World.h"

World::World()
    : socketManager_()
{
    socketManager_.SetOnSocketAdd([&](Websocket* session) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.insert(session);
    });

    socketManager_.SetOnSocketRemove([&](Websocket* session) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.erase(session);
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

void World::Start(std::atomic<bool>* exitSignal, int loopInterval)
{
	auto last_update = std::chrono::steady_clock::now();
	while (!exitSignal->load(std::memory_order_acquire)) {
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_update);
		if (elapsed.count() >= loopInterval) {
			last_update = std::chrono::steady_clock::now();

			this->Update();
		}
	}
}

void World::Update()
{
    spdlog::info("World update #{}", loopCount_++);

    const auto ss = std::string("No small string optimization! :)");

    std::vector<std::weak_ptr<Websocket>> v;
    {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        v.reserve(sessions_.size());
        for (auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for (auto const& wp : v)
        if (auto sp = wp.lock())
            sp->Send(ss);
}