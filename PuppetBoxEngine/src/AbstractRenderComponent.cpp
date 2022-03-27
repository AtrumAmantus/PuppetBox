#include "AbstractRenderComponent.h"
#include "MessageBroker.h"

namespace PB
{
    struct PipelineBoneTransformEvent
    {
        UUID uuid;
        std::uint32_t boneId;
        mat4 transform;
    };

    void AbstractRenderComponent::init() const
    {
        // Set up listener for pipeline updates to bone transforms for a given object by its UUID
        MessageBroker::instance().subscribe("PB_PIPELINE_BONE_TRANSFORM", [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<PipelineBoneTransformEvent>(data);

            std::uint32_t entryIndex = renderDataMap_.at(event->uuid);
            auto& renderData = renderData_.at(entryIndex);
            renderData.boneTransformations[event->boneId] = event->transform;
        });
    }

    void AbstractRenderComponent::render() const
    {
        for (auto& data : renderData_)
        {
            for (auto& model : data.model)
            {
                render(data.boneTransformations, model);
            }
        }
    }
}