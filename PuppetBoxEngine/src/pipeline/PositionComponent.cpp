#include "../GfxMath.h"
#include "../PipelineComponents.h"

namespace PB
{
    void PositionComponent::inits()
    {
        // Set up listener for pipeline updates to add an entity by its UUID
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);
            transformsMap_[event->uuid] = EntityTransform{event->uuid, {}, {}, {1.0f, 1.0f, 1.0f}};
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to position for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock = createLock();

            auto event = std::static_pointer_cast<PipelineSetObjectPositionEvent>(data);
            auto& transform = transformsMap_[event->uuid];
            transform.position = event->position;

            queuedTransforms_.push(transform);
        });

        subscriptions_.push_back(uuid);
    }

    void PositionComponent::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void PositionComponent::updates(float deltaTime)
    {
        Result<EntityTransform> transform = queuedTransforms_.pop();

        while (transform.hasResult)
        {
            auto& t = transform.result;

            auto event = std::make_shared<PipelineEntityTransformEvent>();
            event->uuid = t.uuid;
            event->transform = GfxMath::CreateTransformation(t.rotation, t.scale, t.position);

            MessageBroker::instance().publish(PB_EVENT_PIPELINE_ENTITY_TRANSFORM_TOPIC, event);

            transform = queuedTransforms_.pop();
        }
    }
}