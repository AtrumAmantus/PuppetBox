#pragma once

#include "puppetbox/AbstractBehavior.h"
#include "puppetbox/DataStructures.h"
#include "puppetbox/SceneObject.h"

namespace PB
{
    class WanderBehavior : public AbstractBehavior
    {
    public:
        enum Events {
            START,
            STOP
        };
    public:
        WanderBehavior();

        void init(SceneObject* sceneObject) override;

        void update(SceneObject* sceneObject, float deltaTime) override;

        vec3 getTargetPosition() const override;

    private:
        vec3 targetPosition_{};
        bool behaviorStarted_ = false;
        float waitTime_ = 0;
    };
}