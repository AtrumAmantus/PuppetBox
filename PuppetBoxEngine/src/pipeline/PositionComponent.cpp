#include "../GfxMath.h"
#include "../MessageBroker.h"
#include "../Pipeline.h"
#include "../PipelineEvents.h"

namespace PB
{
    PositionComponent::~PositionComponent() noexcept
    {
        for (const auto& subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void PositionComponent::init()
    {
        auto uuid = MessageBroker::instance().subscribe(
                PB_EVENT_PIPELINE_SET_ENTITY_POSITION_TOPIC,
                [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PipelineSetObjectPositionEvent>(data);

            queuedPositionUpdates_.push({event->uuid, event->position});
        });

        subscriptions_.push_back(uuid);
    }

    void PositionComponent::update(float deltaTime)
    {
        Result<ObjectPositionUpdate> result = queuedPositionUpdates_.pop();

        while (result.hasResult)
        {
            const std::uint32_t VECTOR_INDEX = entityMap().at(result.result.uuid);
            auto transform = transforms_->at(VECTOR_INDEX);

            transform.position = result.result.position;

            singleRenders_->at(VECTOR_INDEX).transform = GfxMath::CreateTransformation(
                transform.rotation,
                transform.scale,
                transform.position
            );

            result = queuedPositionUpdates_.pop();
        }
    }

    void PositionComponent::addVectorReference(std::string referenceName, std::shared_ptr<void> reference)
    {
        if (referenceName == "pb_single_render")
        {
            singleRenders_ = std::move(std::static_pointer_cast<std::vector<SingleRenderData>>(reference));
        }
        else if (referenceName == "pb_transform")
        {
            transforms_ = std::move(std::static_pointer_cast<std::vector<Transform>>(reference));
        }
    }
}