#include <exception>

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

    AbstractRenderComponent::~AbstractRenderComponent() noexcept
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void AbstractRenderComponent::init()
    {
        auto uuid = MessageBroker::instance().subscribe(
                PB_EVENT_PIPELINE_ADD_MODEL_TOPIC,
                [this](std::shared_ptr<void> data) {
                    auto event = std::static_pointer_cast<PipelineAddModelEvent>(data);

                    sync([this, event]() {
                        singleRenders_->at(entityMap_->at(event->uuid)).models.push_back(event->model);
                    });
                });

        subscriptions_.push_back(uuid);
    }

    void AbstractRenderComponent::render() const
    {
        for (const auto& data : *singleRenders_)
        {
            for (auto& model : data.models)
            {
                PB::mat4 transform = data.transformReference->getMat4();

                render(data.boneTransformations, model, transform);
            }
        }

        for (const auto& data : *instanceRenders_)
        {
            renderInstance(
                    data.model,
                    data.transformReference->getMat4(),
                    data.instanceTransforms,
                    data.instanceData);
        }
    }

    void AbstractRenderComponent::addDataVector(std::string referenceName, std::shared_ptr<void> reference)
    {
        if (referenceName == "pb_single_render")
        {
            singleRenders_ = std::move(std::static_pointer_cast<std::vector<SingleRenderData>>(reference));
        }
        else if (referenceName == "pb_instance_render")
        {
            instanceRenders_ = std::move(std::static_pointer_cast<std::vector<InstanceRenderData>>(reference));
        }
    }

    void AbstractRenderComponent::setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap)
    {
        entityMap_ = entityMap;
    }

    void AbstractRenderComponent::sync(const std::function<void()>& process)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        process();
    }

    void AbstractRenderComponent::lock()
    {
        mutex_.lock();
        isLocked_ = true;
    }

    void AbstractRenderComponent::unlock()
    {
        mutex_.unlock();
        isLocked_ = false;
    }
}