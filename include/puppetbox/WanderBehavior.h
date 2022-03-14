#pragma once

#include "AbstractBehavior.h"
#include "DataStructures.h"
#include "SceneObject.h"

namespace PB
{
    class WanderBehavior : public AbstractBehavior
    {
    public:
        enum Events
        {
            START,
            STOP
        };
    public:
        WanderBehavior();

        void update(float deltaTime) override;

        vec3 getTargetPosition() const override;

    protected:
        void inits() override;

    private:
        vec3 targetPosition_{};
        bool behaviorStarted_ = false;
        float waitTime_ = 0;
    };
}