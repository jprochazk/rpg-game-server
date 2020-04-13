
#pragma once
#ifndef SERVER_WORLD_WORLD_H
#define SERVER_WORLD_WORLD_H

#include "common/SharedDefines.h"
#include "world/WorldSocketManager.h"
#include "world/WorldSession.h"
#include "world/WorldTime.h"
#include <unordered_map>

class Websocket;

class World {
public:
    static World* Instance();
    WorldSocketManager* GetSocketManager();

    void StartMainLoop(std::atomic<bool>* exitSignal, float updateRate = 30, int maxConsecutiveUpdates = 5);

    WorldTime::TimePoint GetWorldTime();
    float GetWorldTimestamp();
    std::string GetWorldDate();
private:
    World();
    ~World();

    void Update();

    WorldSocketManager socketManager_;
    
    unsigned loopCount_;
    WorldTime::TimePoint startTime_;
    WorldTime::TimePoint worldTime_;
    
    std::mutex sessionsLock_;
    uint16_t sessionIdSequence_;
    std::unordered_map<uint16_t, std::shared_ptr<WorldSession>> sessions_;
    
    // TODO: finish sessions
    // std::vector<WorldSession> sessions_;
}; // class World


#endif // SERVER_WORLD_WORLD_H