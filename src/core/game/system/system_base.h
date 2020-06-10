#pragma once

#include "game/world.h"
#include <entt/entt.hpp>

namespace game {

class system_base {
public:
	virtual ~system_base() = default;
	virtual void setup(game::world&) = 0;
	virtual void update(game::world&) = 0;
};

} // namespace game
