#pragma once

#include <string>

#include <puppetbox/SceneObject.h>
#include <puppetbox/WanderBehavior.h>

class Entity : public PB::SceneObject
{
public:
    std::string name;
    uint8_t health = 50;
protected:
    void updates(float deltaTime) override
    {
        if (moveVector.y > 0)
        {
            rotation.y = 180;
        }
        else
        {
            rotation.y = 0;
        }

        if (moveVector.x != 0 || moveVector.y != 0)
        {
            if (!isMoving)
            {
                isMoving = true;
                this->playAnimation(Constants::Animation::kWalk, 0);
            }
        }
        else
        {
            if (isMoving)
            {
                isMoving = false;
                this->stopAnimation(Constants::Animation::kWalk);
            }
        }
    };

    void behaviorEvent(std::string behaviorName, std::uint32_t behaviorEvent) override
    {
        if (behaviorName == "wander")
        {
            if (behaviorEvent == PB::WanderBehavior::Events::START)
            {
                this->playAnimation(Constants::Animation::kWalk, 0);

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
                this->stopAnimation(Constants::Animation::kWalk);

                if (rand() % 3 == 1)
                {
                    this->playAnimation(Constants::Animation::kIdle0, 0);
                }
            }
        }
    };
private:
    bool isMoving = false;
};
