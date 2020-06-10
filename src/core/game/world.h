#pragma once


#include "common/assert.h"
#include "common/ordered_map.h"
#include <entt/entt.hpp>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <optional>

#ifndef NDEBUG
#include <boost/type_index.hpp>
#endif

namespace game {

class system_base;

class world {
	entt::registry registry;
	tsl::ordered_map<std::string, std::shared_ptr<system_base>> systems;

public:
	void add_system(const std::string& name, std::shared_ptr<system_base> system);

	template<typename System>
	std::shared_ptr<System> get_system(const std::string& name) {
		auto it = systems.find(name);
		debug_assert(it != systems.end(), "System \"{}\" does not exist", name);
		auto& [_, system] = *it;

#ifndef NDEBUG
		auto ptr = std::dynamic_pointer_cast<System>(system);
		debug_assert(static_cast<bool>(ptr), "System \"{}\" is not of type \"{}\"",
			name, boost::typeindex::type_id<System>().pretty_name());
#else
		auto ptr = std::static_pointer_cast<System>(system);
#endif
		return ptr;
	}

	void remove_system(const std::string& name);
	void update();
	entt::registry& get_registry();
	entt::registry const& get_registry() const;
};

} // namespace game