#include "../PipelineComponents.h"

namespace PB
{
    void AnimationComponent::init()
    {
        // Set up listener for pipeline updates to add an entity by its UUID
        MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = lock();

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);
            animatorMap_[event->uuid] = animators_.size();
            animators_.push_back(EntityAnimator{event->uuid, nullptr});
        });

        // Set up listener for pipeline updates to animators for a given object by its UUID
        MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ANIMATOR_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = lock();

            auto event = std::static_pointer_cast<PipelineAddAnimatorEvent>(data);
            animators_[animatorMap_[event->uuid]].animator = std::move(event->animator);
        });
    }

    void AnimationComponent::update(float deltaTime)
    {

    }
}