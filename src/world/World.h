
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

    void Start(std::atomic<bool>* exitSignal, int loopInterval = 1000);

private:
    World();
    ~World();

    void Update();

    WorldSocketManager socketManager_;

    std::mutex sessionsLock_;
    std::unordered_set<Websocket*> sessions_;
    unsigned loopCount_;
}; // class World


#endif // SERVER_WORLD_WORLD_H