#include "pch.h"
#include "GameObject.h"
#include "core/common/BitMask.h"

GameObject::GameObject(uint16_t id, std::string name)
	: id_(id), name_(std::move(name))
	, position_(), lastPosition_()
	, dirty_(false)
{}

GameObject::~GameObject() {}

void GameObject::Update() {
	auto input = inputFlags_.load(std::memory_order_acquire);

	glm::vec3 positionDelta{ 0,0,0 };
	if (HasBitFlag<uint8_t>(input, MovementState::UP)) {
		positionDelta += glm::vec3{ 0,-1,0 };
	}
	else if (HasBitFlag<uint8_t>(input, MovementState::DOWN)) {
		positionDelta += glm::vec3{ 0,1,0 };
	}

	if (HasBitFlag<uint8_t>(input, MovementState::LEFT)) {
		positionDelta += glm::vec3{ -1,0,0 };
	}
	else if (HasBitFlag<uint8_t>(input, MovementState::RIGHT)) {
		positionDelta += glm::vec3{ 1,0,0 };
	}

	if (positionDelta != glm::vec3{0, 0, 0}) {
		lastPosition_ = position_;
		position_ += positionDelta;

		dirty_ = true;
	}

}

uint16_t GameObject::GetId() {
	return id_;
}
std::string GameObject::GetName() {
	return name_;
}

void GameObject::SetInputFlags(uint8_t ms) {
	inputFlags_.store(ms, std::memory_order_release);
}

glm::vec3 GameObject::GetPosition() {
	return position_;
}

void GameObject::SetPosition(glm::vec3 pos) {
	position_ = pos;
}

bool GameObject::Dirty() {
	return dirty_;
}