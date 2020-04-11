
#include "WorldSession.h"
#include "network/Websocket.h"
#include "world/World.h"

WorldSession::WorldSession(
    uint16_t id, 
    Websocket* socket
)   : id_(id)
    , socket_(socket)
{

}

WorldSession::~WorldSession()
{

}

void WorldSession::Update()
{
    if(auto p = socket_->weak_from_this().lock()) {
        auto msg = std::string("This message was sent by the server at " + World::Instance()->GetWorldDate());
        p->Send(msg);
        if(!p->IsBufferEmpty()) {
            auto buf = p->GetBuffer();
            spdlog::info("Session ID {} received {} packets", id_, buf.size());
            size_t pktCount = 0;
            for(auto& pkt : buf) {
                // TODO test byte buffer Append and Read
                spdlog::info("Packet #{} size: {}", pktCount++, pkt.Size());
            }
        }
    }
}

void WorldSession::SendPacket()
{
    
}

bool WorldSession::CompareSocketPtr(Websocket* that)
{
    return socket_ == that;
}
