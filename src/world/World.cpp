
#include "World.h"

namespace world {

WorldSocketManager::WorldSocketManager()
    : socket_manager()
    , socket_join_callback_(nullptr)
    , socket_leave_callback_(nullptr)
{
    spdlog::info("WorldSocketManager instantiated");
}

void WorldSocketManager::set_on_socket_join(std::function<void(network::websocket*)> callback)
{
    socket_join_callback_ = callback;
}

void WorldSocketManager::set_on_socket_leave(std::function<void(network::websocket*)> callback)
{
    socket_leave_callback_ = callback;
}

void WorldSocketManager::socket_join(network::websocket* session)
{
    if(socket_join_callback_)
        socket_join_callback_(session);
}

void WorldSocketManager::socket_leave(network::websocket* session)
{
    if(socket_leave_callback_)
        socket_leave_callback_(session);
}

World::World()
    : socketManager_()
{
    spdlog::info("World instantiated");

    socketManager_.set_on_socket_join([&](network::websocket* session) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.insert(session);
    });

    socketManager_.set_on_socket_leave([&](network::websocket* session) {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        sessions_.erase(session);
    });
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
    auto const ss = std::make_shared<std::string const>(std::string("hello"));

    std::vector<std::weak_ptr<network::websocket>> v;
    {
        std::lock_guard<std::mutex> lock(sessionsLock_);
        v.reserve(sessions_.size());
        for (auto p : sessions_)
            v.emplace_back(p->weak_from_this());
    }

    for (auto const& wp : v)
        if (auto sp = wp.lock())
            sp->send(ss);
}

}; // namespace world