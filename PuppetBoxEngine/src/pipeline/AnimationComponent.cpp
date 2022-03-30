#include "../PipelineComponents.h"

namespace PB
{
    void AnimationComponent::inits()
    {
        // Set up listener for pipeline updates to add an entity by its UUID
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = lock();

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);
            animatorMap_[event->uuid] = animators_.size();
            BoneMap boneMap{};
            boneMap.addBone("root", "", Bone{});
            animators_.push_back(EntityAnimator{event->uuid, boneMap, nullptr});
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to animators for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = lock();

            auto event = std::static_pointer_cast<PipelineAddAnimatorEvent>(data);
            animators_[animatorMap_[event->uuid]].animator = std::move(event->animator);
        });

        subscriptions_.push_back(uuid);
    }

    void AnimationComponent::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void AnimationComponent::update(float deltaTime)
    {
        for (auto& entityAnimator : animators_)
        {
            entityAnimator.animator->update(deltaTime, entityAnimator.boneMap, {});
        }
    }
}