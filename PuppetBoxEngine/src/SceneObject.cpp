#include <string>
#include <utility>

#include "puppetbox/SceneObject.h"
#include "puppetbox/WanderBehavior.h"

#include "GfxMath.h"

namespace PB
{
    SceneObject::SceneObject() : model_(nullptr)
    {

    }

    SceneObject::SceneObject(std::string id, vec3 baseScale, IModel* model) :
            id_(std::move(id)), baseScale_(baseScale), model_(model)
    {

    }

    void SceneObject::attachTo(SceneObject* sceneObject, const std::string& attachPoint)
    {
        attachedTo_ = sceneObject;
        attachPoint_ = attachPoint;
    }

    mat4 SceneObject::getAbsolutePositionForBone(const std::string& boneName) const
    {
        return GfxMath::CreateTransformation(
                vec4(rotation, 1.0f),
                vec4(scale, 1.0f),
                vec4(position, 1.0f)
        ) * model_->getAbsolutePositionForBone(boneName);
    }

    void SceneObject::update(float deltaTime)
    {
        updates(deltaTime);

        // Behaviors might alter animations, etc, so need to run updates on those first.
        if (behavior_ != nullptr)
        {
            behavior_->update(this, deltaTime);
        }

        if (attachedTo_ != nullptr)
        {
            //TODO: Excessively hacky
            transform_ = attachedTo_->getAbsolutePositionForBone(attachPoint_);
            transform_[0][0] = attachedTo_->scale.x;
            transform_[1][1] = attachedTo_->scale.y;
            transform_[2][2] = attachedTo_->scale.z;
            transform_[3].z = attachedTo_->position.z - 5;
        }
        else
        {
            transform_ = GfxMath::CreateTransformation(
                    vec4(rotation, 1.0f),
                    vec4(actualScale(), 1.0f),
                    vec4(position, 1.0f)
            );
        }

        if (model_ != nullptr)
        {
            model_->update(deltaTime);
        }
    }

    void SceneObject::render()
    {
        model_->render(transform_);
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

    void SceneObject::setBehavior(std::unique_ptr<AbstractBehavior> behavior)
    {
        behavior_ = std::move(behavior);
    }

    void SceneObject::setAnimationCatalogue(IAnimationCatalogue* animationCatalogue)
    {
        animationCatalogue_ = animationCatalogue;
    }

    void SceneObject::playAnimation(const std::string& animationPath, std::uint32_t startFrame)
    {
        std::unique_ptr<IAnimator> animator = animationPath.empty() ? nullptr : animationCatalogue_->get(animationPath);
        model_->playAnimation(std::move(animator), startFrame);
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