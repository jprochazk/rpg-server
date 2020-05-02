
#include "pch.h"
#include "WorldSession.h"
#include "common/ByteBuffer.h"
#include "network/Websocket.h"
#include "world/World.h"
#include "world/Opcode.h"

WorldSession::WorldSession(
    uint16_t id, 
    Websocket* socket
)   : id_(id)
    , socket_(socket)
{
}

WorldSession::~WorldSession()
{
    Close();
}

void WorldSession::Update()
{
    if(auto p = socket_->weak_from_this().lock()) {
        if(!p->IsBufferEmpty()) {
            auto buf = p->GetBuffer();
            auto currTime = World::Instance()->GetTimeStamp();
            for(size_t i = 0, len = buf.size(); i < len; i++) {
                WorldPacket pkt(std::move(buf[i]), currTime);
                if(!pkt.IsValid()) {
                    continue;
                }

                if(auto handler = OPCODE_TABLE[pkt.GetOpcode()])
                    handler(this, pkt);
            }
        }
    }
}

void WorldSession::Close()
{
    if(auto p = socket_->weak_from_this().lock()) {
        p->Close();
    }
}

void WorldSession::SendPacket(WorldPacket packet)
{
    if(auto p = socket_->weak_from_this().lock()) {
        p->Send(packet);
    }
}

bool WorldSession::CheckIdentity(Websocket* that) const noexcept
{
    return socket_ == that;
}

uint16_t WorldSession::GetId() const noexcept
{
    return id_;
}
