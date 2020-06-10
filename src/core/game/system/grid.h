#pragma once


#include <entt/entt.hpp>
#include "game/component/position.h"
#include "game/system/system_base.h"
#include "game/world.h"
#include <unordered_map>
#include <type_traits>
#include <utility>
#include <vector>
#include <boost/functional/hash.hpp>

namespace game {

class grid_base {
public:
	virtual ~grid_base() = default;
	virtual std::vector<entt::entity> query(float x, float y, float r) = 0;
};

namespace system {

class grid : public grid_base, public system_base {
public:
	using size_type = uint32_t;
	using entity_storage_type = std::vector<entt::entity>;
	using position_type = std::pair<size_type, size_type>;
	using map_type = std::unordered_map<position_type, entity_storage_type, boost::hash<position_type>>;

	typedef struct size {
		size_type width, height;
	} size;

	typedef struct cell_size {
		size_type width, height;
	} cell_size;

	grid(size size = { 100, 100 }, cell_size cell_size = { 256, 256 });

	void setup(world& world) override;
	void update(world& world) override;

	std::vector<entt::entity> query(float x, float y, float r) override;
	size get_size() const;
	cell_size get_cell_size() const;
private:
	map_type map;

	size_type width_;
	size_type height_;
	size_type cell_width_;
	size_type cell_height_;
};

} // namespace system
} // namespace game
