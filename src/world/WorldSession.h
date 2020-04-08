
#ifndef SERVER_WORLD_WORLD_SESSION_H
#define SERVER_WORLD_WORLD_SESSION_H


class WorldSession {
public:
    WorldSession();
    ~WorldSession();
    WorldSession(const WorldSession&) = delete; // no copies
    WorldSession& operator=(const WorldSession&) = delete; // no self-assignments

public: // public interface
    void Update(); // tied to the main loop
    void SendPacket(); // sends high-level representations of packets - not raw byte buffers!
};

#endif // SERVER_WORLD_WORLD_SESSION_H
