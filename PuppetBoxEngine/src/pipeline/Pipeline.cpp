#include "puppetbox/Pipeline.h"

#include "../PipelineComponents.h"

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

    void Pipeline::init()
    {
        renderComponent_->init();

        //TODO: This is currently order dependent, revisit
        addComponent(std::make_unique<AIComponent>());
        addComponent(std::make_unique<ActionComponent>());
        addComponent(std::make_unique<PositionComponent>());
        addComponent(std::make_unique<PhysicsComponent>());
        addComponent(std::make_unique<AnimationComponent>());

        // Set up listener for pipeline requests for absolute bone transform references
        auto uuid = MessageBroker::instance().subscribe(PB_EVENT_PIPELINE_GET_ABS_BONE_TRANSFORM_TOPIC, [this](std::shared_ptr<void> data){
            auto event = std::static_pointer_cast<PipelineGetAbsoluteBoneTransformMatrixReferenceEvent>(data);

            auto boneTransformEvent = std::make_shared<PipelineGetBoneTransformReferenceEvent>();
            boneTransformEvent->uuid = event->uuid;
            boneTransformEvent->boneId = event->boneId;
            boneTransformEvent->callback = [event](std::shared_ptr<IValueReference> boneReference) {
                auto modelTransformEvent = std::make_shared<PipelineGetModelTransformReferenceEvent>();
                modelTransformEvent->uuid = event->uuid;
                modelTransformEvent->callback = [event, boneReference = std::move(boneReference)](std::shared_ptr<IValueReference> modelReference) {
                    auto absoluteTransformReference = std::make_shared<AbsoluteBoneTransformMatrixReference>(
                            boneReference,
                            modelReference);

                    event->callback(std::move(absoluteTransformReference));
                };

                MessageBroker::instance().publish(PB_EVENT_PIPELINE_GET_MODEL_LOCATION_TOPIC, modelTransformEvent);
            };

            MessageBroker::instance().publish(PB_EVENT_PIPELINE_GET_BONE_TRANSFORM_TOPIC, boneTransformEvent);
        });

        subscriptions_.push_back(uuid);
    }

    void Pipeline::update(float deltaTime)
    {
        for (auto& objectComponent: objectComponents_)
        {
            objectComponent->update(deltaTime);
        }

        renderComponent_->update(deltaTime);
    }

    void Pipeline::addComponent(std::unique_ptr<AbstractObjectComponent> component)
    {
        component->init();

        objectComponents_.push_back(std::move(component));
    }

    void Pipeline::setRenderComponent(std::unique_ptr<IRenderComponent> renderComponent)
    {
        renderComponent_ = std::move(renderComponent);
    }

    void Pipeline::render() const
    {
        renderComponent_->render();
    }

    void Pipeline::tearDown()
    {
        for (auto subscription : subscriptions_)
        {
            MessageBroker::instance().unsubscribe(subscription);
        }

        for (auto& component : objectComponents_)
        {
            component->tearDown();
        }

        renderComponent_->tearDown();
    }
}