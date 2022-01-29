#pragma once

#include "puppetbox/AbstractBehavior.h"

namespace PB
{
    AbstractBehavior::AbstractBehavior(std::string behaviorName) : behaviorName_(std::move(behaviorName))
    {

    }

    void AbstractBehavior::triggerBehaviorEvent(SceneObject* sceneObject, std::string behaviorEvent)
    {
        sceneObject->behaviorEvent(behaviorName_, behaviorEvent);
    }
}