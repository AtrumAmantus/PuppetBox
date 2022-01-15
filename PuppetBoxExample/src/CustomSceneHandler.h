#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include <PuppetBox.h>
#include <puppetbox/Constants.h>
#include <puppetbox/IAnimationCatalogue.h>
#include <puppetbox/KeyCode.h>
#include <puppetbox/SceneObject.h>

#include "Entity.h"
#include "Sprite.h"

namespace
{
    void insertIntoMap(
            const std::string& k,
            PB::SceneObject* v,
            std::unordered_map<std::string, PB::SceneObject*>& map)
    {
        map.insert(
                std::pair<std::string, PB::SceneObject*>{k, v}
        );
    }
}

class CustomSceneHandler : public PB::AbstractSceneHandler
{
public:
    void setUp() override
    {
        auto* myEntity = new Entity{};

        PB::IAnimationCatalogue* anims = PB::CreateAnimationCatalogue();

        if (PB::CreateSceneObject("Assets1/Sprites/GenericMob", myEntity, PB::LibraryAsset::Type::MODEL_2D))
        {
            myEntity->name = "Fred";
            myEntity->position = PB::vec2{100.0f, 50.0f};
            myEntity->setBehavior(PB::AI::Behavior::WANDER);
            addSceneObject(myEntity);
        }

        if (anims->load("Assets1/Animations/BasicHuman"))
        {
            myEntity->playAnimation(anims->get("walk"));
        }

//		auto* mySprite = new Sprite{};
//
//		if (PB::CreateSceneObject("Assets1/Sprites/Event/Click/Generic", mySprite, PB::LibraryAsset::Type::MODEL_2D))
//		{
//			mySprite->id = "click";
//			mySprite->position = PB::vec2{-100.0f, 50.0f};
//			insertIntoMap(mySprite->id, mySprite, renderLast_);
//		}
    };
protected:
    void processInputs() override
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
//            std::cout << "Mouse Delta: " << std::to_string(input()->mouse.deltaX) << ", " << std::to_string(input()->mouse.deltaY) << std::endl;
//            std::cout << "Mouse Coords: " << std::to_string(input()->mouse.x) << ", " << std::to_string(input()->mouse.y) << std::endl;
        }

        if (input()->window.newWidth != 0 || input()->window.newHeight != 0)
        {
            std::cout << "Window Size: " << std::to_string(input()->window.newWidth) << "x"
                      << std::to_string(input()->window.newHeight) << std::endl;
        }

        PB::vec3 moveVec{};

        if (input()->keyboard.isDown(KEY_UP) || input()->keyboard.isDown(KEY_DOWN))
        {
            moveVec.y = input()->keyboard.isDown(KEY_UP) + (-1 * input()->keyboard.isDown(KEY_DOWN));
        }

        if (input()->keyboard.isDown(KEY_RIGHT) || input()->keyboard.isDown(KEY_LEFT))
        {
            moveVec.x = input()->keyboard.isDown(KEY_RIGHT) + (-1 * input()->keyboard.isDown(KEY_LEFT));
        }

        getCamera()->move(moveVec);

        if (input()->mouse.wheelYDir != 0)
        {
            getCamera()->zoom(static_cast<std::int8_t>(input()->mouse.wheelYDir));
        }
    }
};