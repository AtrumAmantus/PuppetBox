#pragma once

#include <iostream>
#include <memory>
#include <vector>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
#include <puppetbox/KeyCode.h>

#include "Entity.h"

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
	void setUp()
	{
		Entity* myEntity = new Entity{};
		PB::CreateSceneObject("Assets1/Sprites/GenericMob", myEntity, PB::LibraryAsset::Type::MODEL_2D);
		myEntity->name = "Fred";
		myEntity->position += {100.0f, 0.0f, 0.0f};
		myEntity->setBehavior(PB::AI::Behavior::WANDER);
		entities_.push_back(myEntity);
	};
	void update(float deltaTime)
	{
		processInput();

		for (auto e : entities_)
		{
			e->update(deltaTime);
		}
	};
	void render()
	{
		for (auto e : entities_)
		{
			e->render();
		}
	};
private:
	std::vector<Entity*> entities_{};
private:
	void processInput()
	{
		if (input()->keyboard.isPressed(KEY_ESCAPE))
		{
			input()->window.windowClose = true;
		}

		if (input()->keyboard.isPressed(KEY_W))
		{
			std::cout << "'w' key is pressed" << std::endl;
		}

		if (input()->mouse.deltaX != 0 || input()->mouse.deltaY != 0)
		{
			std::cout << "Mouse Coords: " << std::to_string(input()->mouse.deltaX) << ", " << std::to_string(input()->mouse.deltaY) << std::endl;
		}

		if (input()->window.newWidth != 0 || input()->window.newHeight != 0)
		{
			std::cout << "Window Size: " << std::to_string(input()->window.newWidth) << "x" << std::to_string(input()->window.newHeight) << std::endl;
		}
	}
};