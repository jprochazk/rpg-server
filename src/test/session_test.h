#pragma once

#include "game/system/session.h"
#include "game/packet.h"
#include "game/world.h"
#include "game/component/position.h"
#include "game/component/velocity.h"
#include "game/schema/id_generated.h"
#include <gtest/gtest.h>
#include <optional>

namespace session_test {

class handler_table : public game::packet::client::handler_table {
public:
	size_t requests = 0;
	game::packet::client::handler handler_ = { game::packet::client::opcode::MAX, nullptr };

	game::packet::client::handler const& get(uint16_t opcode) override {
		++requests;
		return handler_;
	}
};

class socket : public network::socket_base, public std::enable_shared_from_this<socket> {
public:
	bool closed_ = false;
	std::vector<std::vector<uint8_t>> messages;

	void open() override {
		closed_ = false;
	}
	void close() override {
		closed_ = true;
	}
	void send(std::vector<uint8_t> data) override {
		messages.push_back(data);
	}
	bool closed() const override {
		return closed_;
	}
};

} // namespace test

TEST(session, connection) {
	using namespace game;

	auto table = std::make_shared<session_test::handler_table>();
	auto socket = std::make_shared<session_test::socket>();

	world world;
	auto system = std::make_shared<system::session>(table);
	world.add_system("session", system);

	system->on_open(0, socket->weak_from_this());

	world.update();

	auto& registry = world.get_registry();
	// registry should contain one entity with component session
	size_t entity_count = 0;
	registry.view<component::session>().each([&](const entt::entity&,component::session&) { ++entity_count; });
	EXPECT_EQ(entity_count, static_cast<size_t>(1));

	// we should have also received a packet
	EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
	auto& message = socket->messages.front();
	bool packet_ok = VerifyPacketBuffer(flatbuffers::Verifier(message.data(), message.size()));
	EXPECT_TRUE(packet_ok);
	auto packet = GetPacket(message.data());
	EXPECT_EQ(packet->opcode(), static_cast<uint16_t>(packet::server::opcode::ID));

	bool id_ok = VerifyIdBuffer(flatbuffers::Verifier(packet->buffer()->data(), packet->buffer()->size()));
	EXPECT_TRUE(id_ok);
	auto id = GetId(packet->buffer()->data());

	EXPECT_EQ(id->value(), static_cast<uint32_t>(0));
}

TEST(session, disconnection) {
	using namespace game;

	auto table = std::make_shared<session_test::handler_table>();
	auto socket = std::make_shared<session_test::socket>();

	world world;
	auto system = std::make_shared<system::session>(table);
	world.add_system("session", system);

	system->on_open(0, socket->weak_from_this());
	system->on_close(0);

	world.update();

	auto& registry = world.get_registry();
	// registry should contain no entities
	size_t entity_count = 0;
	registry.view<component::session>().each([&](const entt::entity&, component::session&) { ++entity_count; });
	EXPECT_EQ(entity_count, static_cast<size_t>(0));
}

TEST(session, message) {
	using namespace game;

	auto table = std::make_shared<session_test::handler_table>();
	auto socket = std::make_shared<session_test::socket>();

	world world;
	auto system = std::make_shared<system::session>(table);
	world.add_system("session", system);

	system->on_open(0, socket->weak_from_this());
	flatbuffers::FlatBufferBuilder builder{ 4 };
	std::vector<uint8_t> vec{ 2 };
	auto pkt = CreatePacket(builder, packet::server::opcode::MAX, builder.CreateVector(vec.data(), vec.size()));
	builder.Finish(pkt);
	system->on_message(0, to_vec(builder));

	world.update();

	auto& registry = world.get_registry();
	// registry should contain one entity with component session
	size_t entity_count = 0;
	registry.view<component::session>().each([&](const entt::entity&, component::session&) { ++entity_count; });
	EXPECT_EQ(entity_count, static_cast<size_t>(1));

	// handler_table should've had get() called once
	EXPECT_EQ(table->requests, static_cast<size_t>(1));
}