#pragma once

#include <entt/entt.hpp>

namespace game {

class system {
public:
	virtual ~system() = default;
	virtual void update(entt::registry&) = 0;
};

} // namespace game
