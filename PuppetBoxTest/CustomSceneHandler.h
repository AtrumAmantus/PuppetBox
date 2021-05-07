#pragma once

#include <memory>
#include <vector>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>

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
};