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
    public:
        void update(float deltaTime) override;
    };

    class AIComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    class ActionComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    class PositionComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;
    };

    struct EntityAnimator
    {
        UUID uuid;
        std::unique_ptr<IAnimator> animator;
    };

    class AnimationComponent : public AbstractObjectComponent
    {
    public:
        void update(float deltaTime) override;

        void tearDown() override;

    protected:
        void inits() override;

    private:
        std::unordered_map<UUID, std::uint32_t> animatorMap_{};
        std::vector<EntityAnimator> animators_{};
        std::vector<UUID> subscriptions_{};
    };
}