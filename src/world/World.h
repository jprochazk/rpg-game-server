
#ifndef SERVER_WORLD_WORLD_H
#define SERVER_WORLD_WORLD_H

#include "network/Websocket.h"
#include "world/WorldSocketManager.h"

#include <unordered_set>
#include <spdlog/spdlog.h>

class World {
public:
    static World* Instance();
    WorldSocketManager* GetSocketManager();

    void Update();
private: // components
    WorldSocketManager socketManager_;

private:
    World();
    ~World();

    std::mutex sessionsLock_;
    std::unordered_set<Websocket*> sessions_;
}; // class World


#endif // SERVER_WORLD_WORLD_H