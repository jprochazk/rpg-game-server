
#ifndef SERVER_WORLD_WORLD_H
#define SERVER_WORLD_WORLD_H

#include "../network/websocket.h"

#include <unordered_set>
#include <spdlog/spdlog.h>

namespace world {

class WorldSocketManager : public network::socket_manager {
public:
    WorldSocketManager();
    
    void socket_join(network::websocket* session) override;
    void socket_leave(network::websocket* session) override;

public: // events
    void set_on_socket_join(std::function<void(network::websocket*)> callback);
    void set_on_socket_leave(std::function<void(network::websocket*)> callback);

private: // members
    std::function<void(network::websocket*)> socket_join_callback_;
    std::function<void(network::websocket*)> socket_leave_callback_;
};

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
    std::unordered_set<network::websocket*> sessions_;
}; // class World

}; // namespace world


#endif // SERVER_WORLD_WORLD_H