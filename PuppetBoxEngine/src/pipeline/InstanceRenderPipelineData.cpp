#include "../MessageBroker.h"
#include "../Pipeline.h"
#include "../PipelineEvents.h"

namespace PB
{
    InstanceRenderPipelineData::~InstanceRenderPipelineData() noexcept
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void InstanceRenderPipelineData::init()
    {
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_NEW_INSTANCE_SET_TOPIC, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PipelineNewInstanceSetEvent>(data);

            sync([event,this](){
                const std::uint32_t INDEX = instanceRenderVector_->size();

                instanceRenderVector_->push_back(InstanceRenderData{event->uuid});
                instanceRenderVector_->at(INDEX)
                    .transformReference = std::move(std::make_shared<DefaultInstanceTransformMatrixReference>(
                        event->uuid,
                        entityMap(),
                        *instanceRenderVector_
                    ));
            });
        });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_SET_INSTANCE_SET_TOPIC, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PipelineSetInstanceSetEvent>(data);

            sync([event,this](){
                const std::uint32_t INDEX = entityMap().at(event->uuid);
                auto& entry = instanceRenderVector_->at(INDEX);

                entry.instanceTransforms.clear();
                entry.instanceData.clear();

                for (std::uint32_t i = 0; i < event->instanceCount; ++i)
                {
                    entry.instanceTransforms.push_back(event->instance[0].transform);
                    entry.instanceData.push_back(event->instance[0].data);
                }
            });
        });

        subscriptions_.push_back(uuid);

        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_SET_INSTANCE_MODEL_TOPIC, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PipelineInstanceSetModelEvent>(data);

            sync([event,this](){
                const std::uint32_t INDEX = entityMap().at(event->uuid);
                auto& entry = instanceRenderVector_->at(INDEX);

                entry.model = event->model;
            });
        });

        subscriptions_.push_back(uuid);
    }

    void InstanceRenderPipelineData::addData(const UUID uuid)
    {
        // Do nothing
    }

    void InstanceRenderPipelineData::removeDataAt(std::uint32_t index)
    {
        // Do nothing
    }

    const std::string InstanceRenderPipelineData::getReference() const
    {
        return "pb_instance_render";
    }

    std::shared_ptr<void> InstanceRenderPipelineData::getDataVector() const
    {
        return instanceRenderVector_;
    }
}