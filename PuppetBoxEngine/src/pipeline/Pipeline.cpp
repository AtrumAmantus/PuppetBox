#include "puppetbox/IValueReference.h"

#include "../MessageBroker.h"
#include "../Pipeline.h"
#include "../PipelineEvents.h"

namespace PB
{
    class AbsoluteBoneTransformMatrixReference : public IValueReference
    {
    public:
        AbsoluteBoneTransformMatrixReference(
                std::shared_ptr<IValueReference> boneTransformReference,
                std::shared_ptr<IValueReference> modelTransformReference
        ) : boneTransformReference_(std::move(boneTransformReference)),
        modelTransformReference_(std::move(modelTransformReference))
        {

        }

        mat4 getMat4() const override
        {
            return modelTransformReference_->getMat4() * boneTransformReference_->getMat4();
        }

    private:
        const std::shared_ptr<IValueReference> boneTransformReference_;
        const std::shared_ptr<IValueReference> modelTransformReference_;
    };

    Pipeline::~Pipeline() noexcept
    {

    }

    void Pipeline::init()
    {
        // Set up listener for pipeline requests to add an entity
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_ADD_ENTITY_TOPIC, [this](std::shared_ptr<void> data) {
            auto event = std::static_pointer_cast<PipelineAddEntityEvent>(data);

            entityMap_.insert({event->uuid, entityCount_++});

            for (const auto& entry : pipelineDataMap_)
            {
                entry.second->addDataSync(event->uuid);
            }
        });

        subscriptions_.push_back(uuid);
    }

    void Pipeline::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }
    }

    void Pipeline::update(float deltaTime)
    {
        componentsLocked_ = true;

        for (auto& objectComponent: objectComponents_)
        {
            objectComponent->update(deltaTime);
        }
    }

    void Pipeline::render()
    {
        renderComponent_->render();
    }

    void Pipeline::lockPipeline()
    {
        for (auto& pipelineData: pipelineDataMap_)
        {
            pipelineData.second->lock();
        }

        for (auto& component : objectComponents_)
        {
            component->lock();
        }

        renderComponent_->lock();
    }

    void Pipeline::unlockPipeline()
    {
        for (auto& pipelineData : pipelineDataMap_)
        {
            pipelineData.second->unlock();
        }

        for (auto& component : objectComponents_)
        {
            component->unlock();
        }

        renderComponent_->unlock();
    }

    void Pipeline::addComponent(
            std::unique_ptr<AbstractObjectComponent> component,
            std::vector<std::string> vectorReferences)
    {
        if (!componentsLocked_)
        {
            component->init();

            for (auto& reference: vectorReferences)
            {
                component->addVectorReference(reference, dataVectorMap_.at(reference));
            }

            component->setEntityMap(&entityMap_);

            objectComponents_.push_back(std::move(component));
        }
    }

    void Pipeline::setRenderComponent(
            std::unique_ptr<IRenderComponent> renderComponent,
            const std::vector<std::string>& vectorReferences)
    {
        if (!componentsLocked_)
        {
            for (auto& r: vectorReferences)
            {
                renderComponent->addDataVector(r, dataVectorMap_[r]);
            }

            renderComponent_ = std::move(renderComponent);
        }
    }

    void Pipeline::addPipelineData(std::unique_ptr<AbstractPipelineData> pipelineData)
    {
        std::unique_lock<std::mutex> mlock{mutex_};
        isLocked_ = true;

        pipelineData->setEntityMap(&entityMap_);
        dataVectorMap_[pipelineData->getReference()] = std::move(pipelineData->getDataVector());
        pipelineDataMap_[pipelineData->getReference()] = std::move(pipelineData);

        isLocked_ = false;
    }
}