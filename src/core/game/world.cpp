
#include "game/world.h"
#include "game/system/system_base.h"

namespace game {

void world::add_system(const std::string& name, std::shared_ptr<system_base> system) {
	debug_assert(systems.find(name) == systems.end(), "System \"{}\" already exists", name);

	system->setup(*this);
	systems.insert(std::make_pair(name, system));
}

void world::remove_system(const std::string& name) {
	systems.erase(name);
}

void world::update() {
	spdlog::info("world update");
	for (auto& [_, system] : systems) {
		system->update(*this);
	}

	registry.commit();
}

world::registry_type& world::get_registry() {
	return registry;
}

world::registry_type const& world::get_registry() const {
	return registry;
}

}