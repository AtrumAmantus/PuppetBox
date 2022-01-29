#pragma once

#include <string>

#include "puppetbox/SceneObject.h"

class Entity : public PB::SceneObject
{
public:
    std::string name;
    uint8_t health = 50;
protected:
    void updates(float deltaTime) override
    {

    };

    void behaviorEvent(std::string behaviorName, std::string behaviorEvent) override
    {
        if (behaviorName == "wander")
        {
            if (behaviorEvent == "start")
            {
                this->playAnimation("Assets1/Animations/BasicHuman/Walk", 0, 0);
            }
            else if (behaviorEvent == "stop")
            {
                this->stopAnimation("Assets1/Animations/BasicHuman/Walk");

                if (rand() % 3 == 1)
                {
                    this->playAnimation("Assets1/Animations/BasicHuman/Idle_0", 0, 0);
                }
            }
        }
    };
};
