
#include "game/system/movement.h"
#include <spdlog/spdlog.h>

namespace game {
namespace system {

void movement::setup(world& world) {
}

void movement::update(world& world) {
	// this system is responsible for applying velocity to each entity
	// which has both position and velocity components
	auto& registry = world.get_registry();
	registry.view<component::position, component::velocity>().each(
	[&registry](const entt::entity& e, component::position& pos, const component::velocity& vel) {
		registry.replace<component::position>(
			e, 
			component::position{ pos.x + vel.x, pos.y + vel.y }
		);
	});
}

}
}