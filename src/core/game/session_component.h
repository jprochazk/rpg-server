#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include "network/socket.h"
#include <entt/entt.hpp>

struct session {
	uint32_t id;
	std::weak_ptr<network::socket> socket;
};