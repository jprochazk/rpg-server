
#include "pch.h"
#include "WorldSession.h"
#include "core/network/ByteBuffer.h"
#include "core/network/Websocket.h"
#include "world/World.h"
#include "world/network/packet/WorldPacket.h"
#include "world/opcode/Opcode.h"
#include "world/gobj/GameObject.h"

WorldSession::WorldSession(
    uint16_t id, 
    Websocket* socket
)   : id_(id)
    , socket_(socket)
    , packetBufferMutex_()
    , packetBuffer_()
{
    socket_->SetPacketHandler(this);
    DEBUG_LOG_INFO("Opened session id {}", id_);
}

WorldSession::~WorldSession()
{
    DEBUG_LOG_INFO("Closed session id {}", id_);
    Close();
}

void WorldSession::Handle(std::vector<uint8_t>&& packet) {
    WorldPacket pkt(std::move(packet), sWorld.GetTimeStamp());

    if (!pkt.IsValid()) {
        return;
    }

    DEBUG_LOG_INFO("Session id {} got opcode {}", id_, OpcodeTable::ToString(pkt.GetOpcode()));
    auto& handler = OpcodeTable::Get(pkt.GetOpcode());
    if (handler.IsAsync()) {
        handler(*this, pkt);
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
            OpcodeTable::Get(pkt.GetOpcode())(*this, pkt);
        }
    }
}

void WorldSession::Close()
{
    if(auto p = socket_->weak_from_this().lock()) {
        DEBUG_LOG_INFO("Closing session id {}", id_);

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
