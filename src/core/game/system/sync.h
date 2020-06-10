#pragma once


#include "common/sequence.h"
#include "common/enum_util.h"
#include "game/packet.h"
#include "game/system/system_base.h"
#include "game/system/grid.h"
#include "game/component/position.h"
#include "game/component/velocity.h"
#include "game/component/session.h"
#include <entt/entt.hpp>
#include <cstdint>
#include <cstddef>
#include <memory>
#include <map>
#include <vector>


namespace game {

namespace system {

class sync : public system_base {
	std::shared_ptr<game::grid_base> grid_;
public:
	sync(std::shared_ptr<game::grid_base> grid, float visibility);

	const float visibility_radius;

	virtual void setup(world& world) override;
	virtual void update(world& world) override;
};

} // namespace system
} // namespace game