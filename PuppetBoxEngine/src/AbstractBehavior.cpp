#include "puppetbox/AbstractBehavior.h"

namespace PB
{
    AbstractBehavior::AbstractBehavior(const std::string& behaviorName) : behaviorName_(behaviorName)
    {

    }

    AbstractBehavior::~AbstractBehavior() noexcept
    {

    }

    void AbstractBehavior::init(SceneObject* sceneObject)
    {
        this->sceneObject_ = sceneObject;
        inits();
    }

    void AbstractBehavior::triggerBehaviorEvent(std::uint32_t behaviorEvent)
    {
        sceneObject_->behaviorEvent(behaviorName_, behaviorEvent);
    }

    void AbstractBehavior::inits()
    {

    }

    SceneObject* AbstractBehavior::sceneObject() const
    {
        return sceneObject_;
    }
}