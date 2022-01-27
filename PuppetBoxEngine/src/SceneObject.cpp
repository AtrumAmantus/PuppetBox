#include <string>
#include <utility>

#include "puppetbox/SceneObject.h"

#include "WanderBehavior.h"

namespace PB
{
    SceneObject::SceneObject() : model_(nullptr)
    {

    }

    SceneObject::SceneObject(std::string id, vec3 baseScale, IModel* model) :
            id_(std::move(id)), baseScale_(baseScale), model_(model)
    {

    }

    void SceneObject::update(float deltaTime)
    {
        updates(deltaTime);

        if (model_ != nullptr)
        {
            model_->update(deltaTime);
        }

        if (behavior_ != nullptr)
        {
            behavior_->update(this, deltaTime);
        }
    }

    void SceneObject::render()
    {
        vec3 transform[3] = {position, rotation, actualScale()};
        model_->render(mat3{transform});
    }

    void SceneObject::setBehavior(AI::Behavior behavior)
    {
        switch (behavior)
        {
            case AI::Behavior::WANDER:
                behavior_ = std::make_unique<WanderBehavior>();
                behavior_->init(this);
                break;
            default:
                behavior_ = nullptr;
        }
    }

    void SceneObject::setBehavior(std::unique_ptr<IBehavior> behavior)
    {
        behavior_ = std::move(behavior);
    }

    void SceneObject::playAnimation(std::unique_ptr<IAnimator> animator, std::uint32_t startFrame)
    {
        model_->playAnimation(std::move(animator), startFrame);
    }

    vec3 SceneObject::actualScale() const
    {
        return vec3{
                scale.x * baseScale_.x,
                scale.y * baseScale_.y,
                scale.z * baseScale_.z
        };
    }

    std::string SceneObject::getId() const
    {
        return id_;
    }

    bool SceneObject::operator==(const SceneObject& rhs) const
    {
        return id_ == rhs.id_;
    }

    bool SceneObject::operator!=(const SceneObject& rhs) const
    {
        return !(rhs == *this);
    }
}