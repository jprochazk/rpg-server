#pragma once

#include <entt/entt.hpp>
#include "network/socket.h"
#include <cstdint>
#include <cstddef>
#include <memory>
#include <set>

namespace game {
namespace component {

typedef struct session {
	uint32_t id;
	std::weak_ptr<network::socket_base> socket;
	std::set<entt::entity> known;
} session;

} // namespace component
} // namespace game