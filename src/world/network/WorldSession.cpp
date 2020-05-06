
#include "pch.h"
#include "WorldSession.h"
#include "core/network/ByteBuffer.h"
#include "core/network/Websocket.h"
#include "world/World.h"
#include "world/opcode/Opcode.h"

WorldSession::WorldSession(
    uint16_t id, 
    Websocket* socket
)   : id_(id)
    , socket_(socket)
{
    socket_->SetPacketHandler(this);
}

WorldSession::~WorldSession()
{
    Close();
}

void WorldSession::Handle(ByteBuffer&& packet) {
    WorldPacket pkt(std::move(packet), sWorld.GetTimeStamp());

    if (!pkt.IsValid()) {
        return;
    }

    auto& handler = OpcodeTable::GetHandler(pkt.GetOpcode());
    if (handler.IsAsync()) {
        handler(this, pkt);
    }
    else {
        std::lock_guard<std::mutex> lock(packetBufferMutex_);
        packetBuffer_.push_back(std::move(pkt));
    }

}

void WorldSession::Update()
{
    if(auto p = socket_->weak_from_this().lock()) {
        if(packetBuffer_.empty()) {
            return;
        }

        std::lock_guard<std::mutex> lock(packetBufferMutex_);
        for (auto& pkt : packetBuffer_) {
            OpcodeTable::GetHandler(pkt.GetOpcode())(this, pkt);
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
        p->Send(packet.Contents());
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
