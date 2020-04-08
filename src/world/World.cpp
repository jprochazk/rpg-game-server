
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

void World::Update()
{
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