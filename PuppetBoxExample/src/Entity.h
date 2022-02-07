#pragma once

#include <string>

#include "puppetbox/SceneObject.h"
#include "puppetbox/WanderBehavior.h"

class Entity : public PB::SceneObject
{
public:
    std::string name;
    uint8_t health = 50;
protected:
    void updates(float deltaTime) override
    {

    };

    void behaviorEvent(std::string behaviorName, std::uint32_t behaviorEvent) override
    {
        if (behaviorName == "wander")
        {
            if (behaviorEvent == PB::WanderBehavior::Events::START)
            {
                this->playAnimation("Assets1/Animations/BasicHuman/Walk", 0);

                if (this->getBehavior()->getTargetPosition().y > position.y)
                {
                    rotation.y = 180;
                }
                else
                {
                    rotation.y = 0;
                }
            }
            else if (behaviorEvent == PB::WanderBehavior::Events::STOP)
            {
                this->stopAnimation("Assets1/Animations/BasicHuman/Walk");

                if (rand() % 3 == 1)
                {
                    this->playAnimation("Assets1/Animations/BasicHuman/Idle_0", 0);
                }
            }
        }
    };
};
