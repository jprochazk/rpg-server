#pragma once

#include <entt/entt.hpp>
#include "game/system/system_base.h"
#include "game/component/position.h"
#include "game/component/velocity.h"

namespace game {
namespace system {

class movement : public system_base {
public:
	void setup(world& world) override;
	void update(world& world) override;
};

}
} // namespace game