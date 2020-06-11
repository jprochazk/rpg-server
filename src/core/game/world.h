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
public:
	template<typename Entity>
	class registry : public entt::basic_registry<Entity> {
		friend class world;
		using super = entt::basic_registry<Entity>;

		std::vector<entity_type> destroy_queue_;

		void commit() {
			super::destroy(destroy_queue_.begin(), destroy_queue_.end());
			destroy_queue_.clear();
		}
	public:
		void destroy(const entity_type entity) {
			destroy_queue_.push_back(entity);
		}

		void destroy(const entity_type entity, const version_type version) {
			destroy_queue_.push_back(entity);
		}

		template<typename It>
		void destroy(It first, It last) {
			std::copy(first, last, std::back_inserter(destroy_queue_));
		}
	};

	using registry_type = registry<entt::entity>;
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
	registry_type& get_registry();
	registry_type const& get_registry() const;
private:
	registry_type registry;
	tsl::ordered_map<std::string, std::shared_ptr<system_base>> systems;
};

} // namespace game