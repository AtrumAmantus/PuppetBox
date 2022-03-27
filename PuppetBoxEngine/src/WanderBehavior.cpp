#include "puppetbox/WanderBehavior.h"

#include "GfxMath.h"
#include "puppetbox/Utilities.h"

namespace PB
{
    WanderBehavior::WanderBehavior() : AbstractBehavior("wander")
    {

    }

    void WanderBehavior::update(float deltaTime)
    {
        if (waitTime_ <= 0)
        {
            if (GfxMath::BasicallyEqual(sceneObject()->position, targetPosition_))
            {
                AbstractBehavior::triggerBehaviorEvent(STOP);
                behaviorStarted_ = false;

                waitTime_ = ((3 * RandomUtils::pseudoRand()) + 1);

                float randX = (128 * RandomUtils::pseudoRand()) - 64;
                float randY = (128 * RandomUtils::pseudoRand()) - 64;

                float dirX = randX == 0 ? 1 : (abs(randX) / randX);
                float dirY = randY == 0 ? 1 : (abs(randY) / randY);

                vec3 newPosOffset = vec3{
                        randX + (dirX * 32),
                        randY + (dirY * 32),
                        0
                };

                targetPosition_ = sceneObject()->position + newPosOffset;
            }
            else
            {
                if (!behaviorStarted_)
                {
                    AbstractBehavior::triggerBehaviorEvent(START);
                    behaviorStarted_ = true;
                }

                if (targetPosition_.y > sceneObject()->position.y)
                {
                    sceneObject()->rotation.y = PI;
                }
                else
                {
                    sceneObject()->rotation.y = 0;
                }

                vec3 positionDelta = targetPosition_ - sceneObject()->position;
                float deltaSpeed = sceneObject()->speed * deltaTime;

                if (GfxMath::Dot(positionDelta, positionDelta) < deltaSpeed)
                {
                    sceneObject()->position = targetPosition_;
                }
                else
                {
                    vec3 deltaMove = GfxMath::Normalize(positionDelta) * deltaSpeed;
                    sceneObject()->position += deltaMove;
                }
            }
        }
        else
        {
            waitTime_ -= deltaTime;
        }
    }

    const vec3 WanderBehavior::getTargetPosition() const
    {
        return targetPosition_;
    }

    void WanderBehavior::inits()
    {
        targetPosition_ = sceneObject()->position;
    }
}