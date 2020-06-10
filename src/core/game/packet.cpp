
#include "game/packet.h"
#include "game/component/session.h"
#include "game/component/velocity.h"
#include "game/schema/move_input_generated.h"
#include <optional>
#include <spdlog/spdlog.h>


// signature

namespace game {

// void(entt::registry& registry, const entt::entity& entity, network::byte_buffer&& packet)

// temporary as fuck dont leave this in here dumbass :)
const float player_speed = 10.f;
void handle_move(world& world, const entt::entity& entity, const std::vector<uint8_t>& packet) {
	auto& registry = world.get_registry();

	bool ok = VerifyMoveInputBuffer(flatbuffers::Verifier(packet.data(), packet.size()));
	if (!ok) return;
	auto input = GetMoveInput(packet.data());

	auto& vel = registry.get<component::velocity>(entity);
	vel.y = 0.f; vel.x = 0.f;
	if (input->up())	vel.y += -1.f * player_speed;
	if (input->down())	vel.y += 1.f * player_speed;
	if (input->left())	vel.x += -1.f * player_speed;
	if (input->right())	vel.x += 1.f * player_speed;
}

namespace packet {

namespace client {

handler::handler(uint16_t opcode, handler::function_type fn)
	: opcode(opcode), fn(fn) {}

void handler::operator()(world& world, const entt::entity& entity, const std::vector<uint8_t>& packet) {
	if (!fn) return;
	fn(world, entity, packet);
}

static const std::vector<handler> HANDLER_TABLE = {
	{ opcode::MOVE_INPUT, &handle_move },
	{ opcode::MAX, nullptr }
};

const handler& default_handler_table::get(uint16_t opcode) {
	if (opcode >= opcode::MAX) return HANDLER_TABLE[opcode::MAX];
	return HANDLER_TABLE[opcode];
}

} // namespace client
} // namespace packet
} // namespace game