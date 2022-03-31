#pragma once

#include <memory>
#include <unordered_map>

#include "puppetbox/DataStructures.h"
#include "puppetbox/IAnimationCatalogue.h"
#include "puppetbox/AbstractObjectComponent.h"

#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    class PhysicsComponent : public AbstractObjectComponent
    {
    protected:
        void updates(float deltaTime) override;
    };

    class AIComponent : public AbstractObjectComponent
    {
    protected:
        void updates(float deltaTime) override;
    };

    class ActionComponent : public AbstractObjectComponent
    {
    protected:
        void updates(float deltaTime) override;
    };

    struct EntityTransform
    {
        UUID uuid;
        vec3 position;
        vec3 rotation;
        vec3 scale;
    };

    class PositionComponent : public AbstractObjectComponent
    {
    public:
        void tearDown() override;

    protected:
        void inits() override;

        void updates(float deltaTime) override;

    private:
        std::unordered_map<UUID, EntityTransform> transformsMap_{};
        Concurrent::NonBlocking::Queue<EntityTransform> queuedTransforms_{};
        std::vector<UUID> subscriptions_{};
    };

    struct EntityAnimator
    {
        UUID uuid;
        BoneMap boneMap;
        std::unique_ptr<IAnimator> animator;
    };

    class AnimationComponent : public AbstractObjectComponent
    {
    public:
        void tearDown() override;

    protected:
        void inits() override;

        void updates(float deltaTime) override;

    private:
        std::unordered_map<UUID, std::uint32_t> animatorMap_{};
        std::vector<EntityAnimator> animators_{};
        std::vector<UUID> subscriptions_{};
    };
}