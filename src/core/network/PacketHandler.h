#pragma once
#ifndef SERVER_CORE_NETWORK_PACKET_HANDLER_H
#define SERVER_CORE_NETWORK_PACKET_HANDLER_H

#include "pch.h"

class PacketHandler {
public:
	virtual ~PacketHandler() = default;
	virtual void Handle(std::vector<uint8_t>&& packet) = 0;
};

#endif // SERVER_CORE_NETWORK_PACKET_HANDLER_H