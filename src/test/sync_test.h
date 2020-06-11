#pragma once

#include "game/system/sync.h"
#include "game/system/grid.h"
#include "game/packet.h"
#include "game/schema/state_generated.h"
#include <gtest/gtest.h>

namespace sync_test {
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
}

TEST(sync, created) {
	using namespace game;

	auto socket = std::make_shared<sync_test::socket>();

	world world;

	world.add_system("grid", std::make_shared<system::grid>());
	world.add_system("sync", std::make_shared<system::sync>(world.get_system<system::grid>("grid"), 256.f));

	auto& registry = world.get_registry();
	auto entity = registry.create();
	registry.emplace<component::session>(entity, component::session{ 0,socket });
	registry.emplace<component::position>(entity, component::position{ 0.f, 0.f });

	world.update();

	// socket should've received one packet
	EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
	auto& message = socket->messages.front();
	bool packet_ok = VerifyPacketBuffer(flatbuffers::Verifier(message.data(), message.size()));
	EXPECT_TRUE(packet_ok);
	auto packet = GetPacket(message.data());
	EXPECT_EQ(packet->opcode(), static_cast<uint16_t>(packet::server::opcode::STATE));

	bool state_ok = VerifyStateBuffer(flatbuffers::Verifier(packet->buffer()->data(), packet->buffer()->size()));
	EXPECT_TRUE(state_ok);
	auto state = GetState(packet->buffer()->data());

	EXPECT_EQ(state->entities()->size(), static_cast<uint32_t>(1));
	auto received_entity = state->entities()->Get(0);
	EXPECT_EQ(received_entity->id(), static_cast<uint32_t>(0));
	EXPECT_EQ(received_entity->action(), Action::Create);
	EXPECT_EQ(received_entity->components()->position()->x(), 0.f);
	EXPECT_EQ(received_entity->components()->position()->y(), 0.f);
}

TEST(sync, updated) {
	using namespace game;

	auto socket = std::make_shared<sync_test::socket>();

	world world;

	world.add_system("grid", std::make_shared<system::grid>());
	world.add_system("sync", std::make_shared<system::sync>(world.get_system<system::grid>("grid"), 256.f));

	auto& registry = world.get_registry();
	auto entity = registry.create();
	registry.emplace<component::session>(entity, component::session{ 0,socket });
	registry.emplace<component::position>(entity, component::position{ 0.f, 0.f });

	world.update();

	socket->messages.clear();
	registry.replace<component::position>(entity, component::position{ 10.f, 0.f });

	world.update();

	EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
	auto& message = socket->messages.front();
	bool packet_ok = VerifyPacketBuffer(flatbuffers::Verifier(message.data(), message.size()));
	EXPECT_TRUE(packet_ok);
	auto packet = GetPacket(message.data());
	EXPECT_EQ(packet->opcode(), static_cast<uint16_t>(packet::server::opcode::STATE));

	bool state_ok = VerifyStateBuffer(flatbuffers::Verifier(packet->buffer()->data(), packet->buffer()->size()));
	EXPECT_TRUE(state_ok);
	auto state = GetState(packet->buffer()->data());

	EXPECT_EQ(state->entities()->size(), static_cast<uint32_t>(1));
	auto received_entity = state->entities()->Get(0);
	EXPECT_EQ(received_entity->id(), static_cast<uint32_t>(0));
	EXPECT_EQ(received_entity->action(), Action::Update);
	EXPECT_EQ(received_entity->components()->position()->x(), 10.f);
	EXPECT_EQ(received_entity->components()->position()->y(), 0.f);
}

TEST(sync, deleted) {
	using namespace game;

	auto socket = std::make_shared<sync_test::socket>();

	world world;

	world.add_system("grid", std::make_shared<system::grid>());
	world.add_system("sync", std::make_shared<system::sync>(world.get_system<system::grid>("grid"), 256.f));

	auto& registry = world.get_registry();
	auto sentity = registry.create();
	registry.emplace<component::session>(sentity, component::session{ 0,socket });
	registry.emplace<component::position>(sentity, component::position{ 0.f, 0.f });

	auto entity = registry.create();
	registry.emplace<component::position>(entity, component::position{ 0.f, 0.f });

	world.update();

	socket->messages.clear();
	registry.destroy(entity);

	world.update();

	socket->messages.clear();
	world.update();

	EXPECT_EQ(socket->messages.size(), static_cast<size_t>(1));
	auto& message = socket->messages.front();

	auto verifier = flatbuffers::Verifier(message.data(), message.size());
	bool packet_ok = VerifyPacketBuffer(verifier);
	EXPECT_TRUE(packet_ok);
	auto packet = GetPacket(message.data());

	EXPECT_EQ(packet->opcode(), static_cast<uint16_t>(packet::server::opcode::STATE));

	verifier = flatbuffers::Verifier(packet->buffer()->data(), packet->buffer()->size());
	bool state_ok = VerifyStateBuffer(verifier);
	EXPECT_TRUE(state_ok);
	auto state = GetState(packet->buffer()->data());

	EXPECT_EQ(state->entities()->size(), static_cast<uint32_t>(2));
	auto received_sentity = state->entities()->Get(0);
	EXPECT_EQ(received_sentity->id(), static_cast<uint32_t>(0));
	EXPECT_EQ(received_sentity->action(), Action::Update);
	EXPECT_EQ(received_sentity->components()->position()->x(), 0.f);
	EXPECT_EQ(received_sentity->components()->position()->y(), 0.f);

	auto received_entity = state->entities()->Get(1);
	EXPECT_EQ(received_entity->id(), static_cast<uint32_t>(1));
	EXPECT_EQ(received_entity->action(), Action::Delete);
}