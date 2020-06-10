
#include "game/system/grid.h"
#include <algorithm>

namespace game { 
namespace system { 

grid::grid(size size, cell_size cell_size)
	: width_(size.width)
	, height_(size.height)
	, cell_width_(cell_size.width)
	, cell_height_(cell_size.height)
{
	// pre-allocate the grid
	for (size_type y = 0; y < height_; y++) {
		for (size_type x = 0; x < width_; x++) {
			map.insert({ {x,y}, {} });
		}
	}
}

void grid::setup(world& world) {}

void grid::update(world& world) {
	// TODO maybe check if position changed
	// instead of clearing on each update

	// clear each cell's entities
	for (auto& it : map) {
		auto& [position, entities] = it;
		entities.clear();
	}

	// then iterate through every entity with component::position
	// and add it to a grid cell

	auto min_x = 0.f;
	auto min_y = 0.f;
	auto max_x = static_cast<float>(width_) * static_cast<float>(cell_width_);
	auto max_y = static_cast<float>(height_) * static_cast<float>(cell_height_);

	auto& registry = world.get_registry();
	registry.view<component::position>().each(
	[&](const entt::entity& entity, component::position& pos) {
		// we don't want to grow the map, so clamp position to the grid
		pos.x = std::clamp(pos.x, min_x, max_x);
		pos.y = std::clamp(pos.y, min_y, max_y);

		auto& entities = map.at(std::make_pair(
			static_cast<size_type>(pos.x) / cell_width_,
			static_cast<size_type>(pos.y) / cell_height_
		));
		entities.push_back(entity);
	});
}

std::vector<entt::entity> grid::query(float x, float y, float r)
{
	std::vector<entt::entity> found;

	size_type min_x = (x <= r) ? 0 : static_cast<size_type>(std::floorf((x - r) / static_cast<float>(cell_width_)));
	size_type min_y = (y <= r) ? 0 : static_cast<size_type>(std::floorf((y - r) / static_cast<float>(cell_height_)));
	size_type max_x = static_cast<size_type>(std::ceilf((x + r) / static_cast<float>(cell_width_)));
	size_type max_y = static_cast<size_type>(std::ceilf((y + r) / static_cast<float>(cell_height_)));

	auto inserter = std::back_inserter(found);
	for (auto y = min_y; y < max_y; y++) {
		for (auto x = min_x; x < max_x; x++) {
			auto cell_pos = std::make_pair(x, y);
			auto& entities = map.at(cell_pos);

			std::copy(entities.begin(), entities.end(), inserter);
		}
	}

	return found;
}

grid::size grid::get_size() const {
	return size{ width_, height_ };
}
grid::cell_size grid::get_cell_size() const {
	return cell_size{ cell_width_, cell_height_ };
}

} 
}