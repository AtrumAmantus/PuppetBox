#include <string>
#include <utility>

#include "puppetbox/SceneObject.h"
#include "puppetbox/WanderBehavior.h"

#include "GfxMath.h"
#include "Logger.h"

namespace PB
{
    SceneObject::SceneObject() : model_(nullptr)
    {

    }

    SceneObject::SceneObject(UUID uuid, vec3 baseScale, IModel* model)
            : id_(uuid), baseScale_(baseScale), model_(model)
    {

    }

    void SceneObject::attachTo(SceneObject* sceneObject, const std::string& attachPoint)
    {
        attachedTo_ = sceneObject;
        attachPoint_ = attachPoint;
    }

    bool SceneObject::isAttached() const
    {
        return attachedTo_ != nullptr;
    }

    const SceneObject& SceneObject::getAttached() const
    {
        return *attachedTo_;
    }

    mat4 SceneObject::getAbsolutePositionForBone(const std::string& boneName) const
    {
        return GfxMath::CreateTransformation(rotation, scale, position) * model_->getAbsolutePositionForBone(boneName);
    }

    void SceneObject::update(float deltaTime)
    {
        updates(deltaTime);

        vec3 finalVelocity = velocity + (moveVector * speed);
        position += (finalVelocity * deltaTime);

        // Behaviors might alter animations, etc, so need to run updates on those first.
        if (behaviorToAdd_ != nullptr)
        {
            LOGGER_DEBUG("Behavior added to scene object");
            behavior_ = std::move(behaviorToAdd_);
            behaviorToAdd_ = nullptr;
            behavior_->init(this);
            behavior_->update(deltaTime);
        }
        else if (clearBehavior_)
        {
            LOGGER_DEBUG("Behavior removed from scene object");
            clearBehavior_ = false;
            behavior_ = nullptr;
        }
        else if (behavior_ != nullptr)
        {
            behavior_->update(deltaTime);
        }

        if (attachedTo_ != nullptr)
        {
            transform_ = attachedTo_->getAbsolutePositionForBone(attachPoint_);
        }
        else
        {
            transform_ = GfxMath::CreateTransformation(rotation, scale, position);
        }

        if (model_ != nullptr)
        {
            model_->update(deltaTime);
        }

        isUpdated = true;
    }

    void SceneObject::render()
    {
        if (model_ != nullptr)
        {
            model_->render(transform_);
        }

        isUpdated = false;
    }

    void SceneObject::overrideBoneRotation(const std::string& boneName, vec3 rotation)
    {
        if (model_ != nullptr)
        {
            model_->overrideBoneRotation(boneName, rotation);
        }
    }

    void SceneObject::clearBoneOverrides(const std::string& boneName)
    {
        if (model_ != nullptr)
        {
            model_->clearBoneOverrides(boneName);
        }
    }

    void SceneObject::setBehavior(AI::Behavior behavior)
    {
        switch (behavior)
        {
            case AI::Behavior::WANDER:
                behaviorToAdd_ = std::make_unique<WanderBehavior>();
                break;
            default:
                behaviorToAdd_ = nullptr;
        }
    }

    void SceneObject::setBehavior(std::unique_ptr<AbstractBehavior> behavior)
    {
        behaviorToAdd_ = std::move(behavior);
    }

    void SceneObject::clearBehavior()
    {
        clearBehavior_ = true;
    }

    const AbstractBehavior* SceneObject::getBehavior() const
    {
        return &(*behavior_);
    }

    void SceneObject::playAnimation(const std::string& animationPath, std::uint32_t startFrame)
    {
        model_->playAnimation(animationPath, startFrame);
    }

    void SceneObject::stopAnimation()
    {
        model_->stopAnimation();
    }

    void SceneObject::stopAnimation(const std::string& animationPath)
    {
        model_->stopAnimation(animationPath);
    }

    bool SceneObject::isAnimating() const
    {
        return model_->isAnimating();
    }

    vec3 SceneObject::actualScale() const
    {
        //TODO: check "base scale" implementation, this should be model asset dependent.
        return vec3{
                scale.x * baseScale_.x,
                scale.y * baseScale_.y,
                scale.z * baseScale_.z
        };
    }

    BoneMap SceneObject::getBones() const
    {
        return model_->getBones();
    }

    const UUID SceneObject::getId() const
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
