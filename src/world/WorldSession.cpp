
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

typedef struct WriteTest {
    float       timestamp;
    std::string date;
    std::string message;
} WriteTest;
ByteBuffer& operator<<(ByteBuffer& buf, const WriteTest& data) {
    buf.Reserve(4 + 1 + data.date.length() + 1 + data.message.length());
    buf << data.timestamp;
    buf << data.date;
    buf << data.message;
    return buf;
}
typedef struct ReadTest {
    std::optional<uint32_t>    sequence;
    std::optional<std::string> message;
} ReadTest;
ByteBuffer& operator>>(ByteBuffer& buf, ReadTest& data) {
    buf >> data.sequence;
    buf >> data.message;
    return buf;
}

void WorldSession::Update()
{
    if(auto p = socket_->weak_from_this().lock()) {
        WriteTest write_test = { 
            World::Instance()->GetWorldTimestamp(), 
            World::Instance()->GetWorldDate(), 
            "Hello from the server!" 
        };
        ByteBuffer msg;
        msg << write_test;

        p->Send(msg);
        if(!p->IsBufferEmpty()) {
            auto buf = p->GetBuffer();
            spdlog::info("Session ID {} received {} packets", id_, buf.size());
            size_t pktCount = 0;
            for(auto& pkt : buf) {
                ReadTest read_test;
                pkt >> read_test;
                spdlog::info("Packet #{} size: {}. Data: {{ sequence: {}, message: {} }}", 
                    pktCount++, pkt.Size(), 
                    read_test.sequence.value_or(0), read_test.message.value_or("")
                );
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
