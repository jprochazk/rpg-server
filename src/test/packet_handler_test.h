#pragma once

#include "game/packet.h"
#include "game/world.h"
#include "game/component/velocity.h"
#include "game/schema/move_input_generated.h"
#include <gtest/gtest.h>

TEST(packet_handler_test, move_input) {
	using namespace game::packet;
	using namespace game::component;

	game::world world;
	auto& registry = world.get_registry();

	auto entity = registry.create();
	registry.emplace<velocity>(entity);

	client::default_handler_table table;

	auto handler = table.get(client::opcode::MOVE_INPUT);

	auto vel_case = [&](uint8_t input, float e_x, float e_y) {
		flatbuffers::FlatBufferBuilder builder;
		auto move_input = CreateMoveInput(builder,
			(input & 1) == 1,
			(input & 2) == 2,
			(input & 4) == 4,
			(input & 8) == 8);
		builder.Finish(move_input);
		handler(world, entity, game::to_vec(builder));
		auto& vel = registry.get<velocity>(entity);
		EXPECT_EQ(vel.y, e_y);
		EXPECT_EQ(vel.x, e_x);
		vel.x = 0.f; vel.y = 0.f;
	};

	vel_case(1,0,-1);
	vel_case(2,0,1);
	vel_case(3,0,0);
	vel_case(4,-1,0);
	vel_case(5,-1,-1);
	vel_case(6,-1,1);
	vel_case(7,-1,0);
	vel_case(8,1,0);
	vel_case(9,1,-1);
	vel_case(10,1,1);
	vel_case(11,1,0);
	vel_case(12,0,0);
}
