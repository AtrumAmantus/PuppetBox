#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
#include <puppetbox/KeyCode.h>
#include <puppetbox/SceneObject.h>

#include "Entity.h"
#include "Sprite.h"

namespace
{
	void insertIntoMap(std::string k, PB::SceneObject* v, std::unordered_map <std::string, PB::SceneObject*>& map)
	{
		map.insert(
			std::pair<std::string, PB::SceneObject*>{ k, v }
		);
	}
}

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
	void setUp()
	{
		Entity* myEntity = new Entity{};
		if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", myEntity, PB::LibraryAsset::Type::MODEL_2D))
		{
			myEntity->id = "Fred";
			myEntity->name = myEntity->id;
			myEntity->position += {0.0f, 0.0f, 0.0f};
			myEntity->setBehavior(PB::AI::Behavior::WANDER);
			insertIntoMap(myEntity->id, myEntity, entities_);
		}
	};
	void update(float deltaTime)
	{
		processInput();

		for (auto& e : entities_)
		{
			e.second->update(deltaTime);
		}
	};
	void render()
	{
		for (auto& e : entities_)
		{
			e.second->render();
		}

		for (auto& e : renderLast_)
		{
			e.second->render();
		}
	};
private:
	std::unordered_map<std::string, PB::SceneObject*> entities_{};
	std::unordered_map<std::string, PB::SceneObject*> renderLast_{};
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
			//std::cout << "Mouse Coords: " << std::to_string(input()->mouse.deltaX) << ", " << std::to_string(input()->mouse.deltaY) << std::endl;
		}

		if (input()->window.newWidth != 0 || input()->window.newHeight != 0)
		{
			std::cout << "Window Size: " << std::to_string(input()->window.newWidth) << "x" << std::to_string(input()->window.newHeight) << std::endl;
		}
	}
};