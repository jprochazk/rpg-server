#pragma once
#ifndef SERVER_WORLD_GOBJ_GAMEOBJECT_H
#define SERVER_WORLD_GOBJ_GAMEOBJECT_H

#include "pch.h"
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

class GameObject {
public:
	enum MovementState : uint8_t {
		UP		= 0x01,
		DOWN	= 0x02,
		LEFT	= 0x04,
		RIGHT	= 0x08
	};

	GameObject(uint16_t, std::string);
	~GameObject();

	void		Update();

	uint16_t	GetId();
	std::string GetName();

	void		SetInputFlags(uint8_t);

	glm::vec3	GetPosition();
	void		SetPosition(glm::vec3);

	bool		Dirty();


protected:
	uint16_t	id_;
	std::string	name_;

	std::atomic<uint8_t> inputFlags_;

	glm::vec3 position_;
	glm::vec3 lastPosition_;

	bool dirty_;
};

#endif // SERVER_WORLD_GOBJ_GAMEOBJECT_H