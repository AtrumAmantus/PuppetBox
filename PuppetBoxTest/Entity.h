#pragma once

#include <string>

#include "../include/puppetbox/SceneObject.h"

class Entity : public PB::SceneObject
{
public:
	std::string name;
	uint8_t health = 50;
public:
	void update(float deltaTime)
	{

	};
};