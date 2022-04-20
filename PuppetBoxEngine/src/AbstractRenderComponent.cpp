#include "puppetbox/IValueReference.h"

#include "AbstractRenderComponent.h"
#include "MessageBroker.h"
#include "PipelineEvents.h"

namespace PB
{
    class TransformInstanceReference : public IValueReference
    {
    public:
        TransformInstanceReference(
                const UUID uuid,
                const std::vector<InstanceRenderData>& renderData,
                const std::unordered_map<UUID, std::uint32_t>& renderDataMap
        ) : uuid_(uuid), renderData_(renderData), renderDataMap_(renderDataMap)
        {

        };

    private:
        mat4 getMat4() const override
        {
            return renderData_[renderDataMap_.at(uuid_)].transform;
        }

    private:
        const UUID uuid_;
        const std::vector<InstanceRenderData>& renderData_{};
        const std::unordered_map<UUID, std::uint32_t>& renderDataMap_{};

    };

    class TransformReference : public IValueReference
    {
    public:
        TransformReference(
                const UUID uuid,
                const std::vector<SingleRenderData>& renderData,
                const std::unordered_map<UUID, std::uint32_t>& renderDataMap
        ) : uuid_(uuid), renderData_(renderData), renderDataMap_(renderDataMap)
        {

        };

    private:
        mat4 getMat4() const override
        {
            return renderData_[renderDataMap_.at(uuid_)].transform;
        }

    private:
        const UUID uuid_;
        const std::vector<SingleRenderData>& renderData_{};
        const std::unordered_map<UUID, std::uint32_t>& renderDataMap_{};

    };

    void AbstractRenderComponent::init()
    {
        // Set up listener for pipeline entity additions by its UUID
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);

            singleRenderDataMap_[event->uuid] = singleRenderData_.size();
            std::vector<mat4> boneTransforms{};
            boneTransforms.push_back(mat4::eye());
            auto renderData = SingleRenderData{
                event->uuid,
                mat4::eye(),
                nullptr,
                std::move(boneTransforms),
                {}};
            renderData.transformReference = std::make_shared<TransformReference>(
                    renderData.uuid,
                    singleRenderData_,
                    singleRenderDataMap_);
            singleRenderData_.push_back(renderData);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline models additions by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_MODEL_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddModelEvent>(data);

            singleRenderData_[singleRenderDataMap_[event->uuid]].models.push_back(event->model);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to bone transforms for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_BONE_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineBoneTransformEvent>(data);

            singleRenderData_[singleRenderDataMap_[event->uuid]].boneTransformations = std::move(event->transforms);
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to entity transforms for a given object by its UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ENTITY_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineEntityTransformEvent>(data);

            auto& renderData = singleRenderData_[singleRenderDataMap_[event->uuid]];
            renderData.transform = event->transform;
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to attach commands for a given UUID
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ATTACH_OBJECT_TO_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAttachObjectTo>(data);

            LOGGER_INFO("Setting " + std::to_string(event->parasiteUUID) + " to reference");

            singleRenderData_[singleRenderDataMap_[event->parasiteUUID]].transformReference = event->reference;
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to add instances sets
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_INSTANCE_SET_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddInstanceSetEvent>(data);

            instanceRenderDataMap_[event->uuid] = instanceRenderData_.size();
            instanceRenderData_.push_back({
                event->uuid,
                event->transform,
                std::make_shared<TransformInstanceReference>(
                        event->uuid,
                        instanceRenderData_,
                        instanceRenderDataMap_),
                event->model
            });
        });

        subscriptions_.push_back(uuid);

        // Set up listener for pipeline updates to add instances to instance sets
        uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_INSTANCE_TOPIC, [this](std::shared_ptr<void> data){
            std::unique_lock<std::mutex> mlock{mutex_};

            auto event = std::static_pointer_cast<PipelineAddInstanceEvent>(data);

            auto& set = instanceRenderData_[instanceRenderDataMap_[event->setUUID]];
            set.instanceMap[event->instanceUUID] = set.instanceTransforms.size();
            set.instanceTransforms.push_back(event->transform);
            set.instanceData.push_back(event->data);
            //TODO: This seems like it will be really slow, I think I need to have the pipeline components all
            // accessing a single data set rather then passing information through events from one to the next.
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

        for (auto& data : singleRenderData_)
        {
            for (auto& model : data.models)
            {
                PB::mat4 transform = data.transformReference->getMat4();

                render(data.boneTransformations, model, transform);
            }
        }

        for (auto& data : instanceRenderData_)
        {
            renderInstance(
                    data.model,
                    data.transformReference->getMat4(),
                    data.instanceTransforms,
                    data.instanceData);
        }
    }
}