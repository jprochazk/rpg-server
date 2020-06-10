#pragma once

#include "game/system/grid.h"
#include "game/component/position.h"
#include <gtest/gtest.h>
#include <vector>

TEST(grid, query) {
	game::world world;
	auto grid = std::make_shared<game::system::grid>();
	world.add_system("grid", grid);

	auto& registry = world.get_registry();
	auto e1 = registry.create();
	auto e2 = registry.create();
	auto e3 = registry.create();
	registry.emplace<game::component::position>(e1, game::component::position{ 127.f, 127.f });
	registry.emplace<game::component::position>(e2, game::component::position{ 383.f, 383.f });
	registry.emplace<game::component::position>(e3, game::component::position{ 639.f, 639.f });

	world.update();

	std::vector<entt::entity> expected = {
		e1, e2, e3 
	};
	std::vector<entt::entity> actual = grid->query(384.f, 384.f, 256.f);
	EXPECT_EQ(expected.size(), actual.size());

	std::set<entt::entity> e_set{ expected.begin(), expected.end() };
	std::set<entt::entity> a_set{ actual.begin(), actual.end() };
	EXPECT_EQ(a_set, e_set);
}