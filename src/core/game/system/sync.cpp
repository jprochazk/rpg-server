
#include "game/system/sync.h"
#include "game/schema/packet_generated.h"
#include "game/schema/state_generated.h"
#include <flatbuffers/flatbuffers.h>
#include <spdlog/spdlog.h>

inline flatbuffers::Offset<Entity> serialize_entity(
	flatbuffers::FlatBufferBuilder& builder,
	entt::registry& registry,
	const entt::entity& entity,
	Action action
) {
	auto id = static_cast<uint32_t>(entity);

	flatbuffers::Offset<Components> components{ 0 };
	if (action != Action::Delete) {
		auto* pos_component = registry.try_get<game::component::position>(entity);

		std::optional<flatbuffers::Offset<Position>> pos;
		if (pos_component) pos = CreatePosition(builder, pos_component->x, pos_component->y);

		ComponentsBuilder cb{ builder };
		if (pos) cb.add_position(*pos);
		components = cb.Finish();
	}

	auto serialized = CreateEntity(builder,
		id,
		action,
		components
	);
	builder.Finish(serialized);
	return serialized;
}

namespace game {
namespace system {

sync::sync(
	std::shared_ptr<game::grid_base> grid,
	float visibility
)	: grid_(grid)
	, visibility_radius(visibility) 
{}

void sync::setup(world& world) {}

void sync::update(world& world) {

	auto& registry = world.get_registry();
	registry.view<component::session, component::position>().each(
	[&](const entt::entity& e, component::session& session, const component::position& position) {
		/*spdlog::info("sync update entity {}, sid {}, position {},{}", 
			static_cast<uint32_t>(e), session.id, position.x, position.y);*/
		std::shared_ptr<network::socket_base> sock = session.socket.lock();
		if (!sock) return;

		// TEMP many small allocations here, could quickly become a bottleneck
		// TODO custom allocator?

		auto view = grid_->query(position.x, position.y, visibility_radius);
		std::set<entt::entity> deleted{ session.known.begin(), session.known.end() };
		// look for entities which are also in view and erase those
		for (const auto& e : view) {
			if (deleted.find(e) != deleted.end()) {
				deleted.erase(e);
			}
		}

		flatbuffers::FlatBufferBuilder builder{ 32 + view.size() * 64 + deleted.size() * 32 };
		std::vector<flatbuffers::Offset<Entity>> entities;
		entities.reserve(view.size() + deleted.size());
		// created or updated entities
		for (const auto& entity : view) {
			flatbuffers::Offset<Entity> serialized;
			if (session.known.find(entity) == session.known.end()) {
				session.known.insert(entity);
				serialized = serialize_entity(builder, registry, entity, Action::Create);
			}
			else {
				serialized = serialize_entity(builder, registry, entity, Action::Update);
			}
			entities.push_back(serialized);
		}
		// deleted entities
		for (const auto& entity : deleted) {
			session.known.erase(entity);
			auto serialized = serialize_entity(builder, registry, entity, Action::Delete);
			entities.push_back(serialized);
		}
		auto state = CreateState(builder, builder.CreateVector<Entity>(entities.data(), entities.size()));
		builder.Finish(state);

		sock->send(packet::build(packet::server::opcode::STATE, to_vec(builder)));
	});
}

} // namespace component
} // namespace game