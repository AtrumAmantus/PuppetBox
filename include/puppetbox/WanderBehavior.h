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

    private:
        vec3 targetPosition_{};
        float waitTime_ = 0;
    };
}