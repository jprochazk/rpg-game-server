
#include "WorldSession.h"
#include "common/ByteBuffer.h"
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
        ByteBuffer msg(30);
        msg.Write<uint8_t>(1);
        msg.Write<uint16_t>(1);
        msg.Write<uint32_t>(1);
        msg.Write<uint64_t>(1);
        msg.Write<int8_t>(1);
        msg.Write<int16_t>(1);
        msg.Write<int32_t>(1);
        msg.Write<int64_t>(1);
        msg.Write<float>(1);
        msg.Write<double>(1);
        p->Send(msg);
        if(!p->IsBufferEmpty()) {
            auto buf = p->GetBuffer();
            spdlog::info("Session ID {} received {} packets", id_, buf.size());
            size_t pktCount = 0;
            for(auto& pkt : buf) {
                spdlog::info("Packet #{} size: {}", pktCount++, pkt.Size());
            }
        }
    } else { 
        // happens if socket is closed and session still updates
        // just silently ignore
    }
}

void WorldSession::SendPacket()
{
    
}

bool WorldSession::CompareSocketPtr(Websocket* that)
{
    return socket_ == that;
}
