#include "AbstractRenderComponent.h"
#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    void AbstractRenderComponent::init()
    {
        // Set up listener for pipeline entity additions by its UUID
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);

            renderDataMap_[event->uuid] = renderData_.size();
            std::vector<mat4> boneTransforms{};
            boneTransforms.push_back(mat4::eye());
            renderData_.push_back(RenderData{event->uuid, mat4::eye(), std::move(boneTransforms), {}});
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline model additions by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_MODEL_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddModelEvent>(data);

            renderData_[renderDataMap_[event->uuid]].model.push_back(event->model);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to bone transforms for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineBoneTransformEvent>(data);

            renderData_[renderDataMap_[event->uuid]].boneTransformations = std::move(event->transforms);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to bone transforms for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ENTITY_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineEntityTransformEvent>(data);

            renderData_[renderDataMap_[event->uuid]].transform = event->transform;
        });

        subscriptions_.push_back(uuid);
    }

    void AbstractRenderComponent::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void AbstractRenderComponent::render()
    {
        // Lock the component while renders are running
        std::unique_lock<std::mutex> mlock{mutex_};

        for (auto& data : renderData_)
        {
            for (auto& model : data.model)
            {
                render(data.boneTransformations, model, data.transform);
            }
        }
    }
}