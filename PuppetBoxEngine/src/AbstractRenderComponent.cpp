#include "AbstractRenderComponent.h"
#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    void AbstractRenderComponent::init()
    {
        // Set up listener for pipeline entity additions by its UUID
        MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);

            renderDataMap_[event->uuid] = renderData_.size();
            renderData_.push_back(RenderData{event->uuid, {}, {}});
        });

        // Set up listener for pipeline model additions by its UUID
        MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_MODEL_TOPIC, [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<PipelineAddModelEvent>(data);

            renderData_[renderDataMap_[event->uuid]].model.push_back(event->model);
        });

        // Set up listener for pipeline updates to bone transforms for a given object by its UUID
        MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<PipelineBoneTransformEvent>(data);

            std::uint32_t entryIndex = renderDataMap_.at(event->uuid);
            auto& renderData = renderData_.at(entryIndex);
            renderData.boneTransformations = event->transforms;
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