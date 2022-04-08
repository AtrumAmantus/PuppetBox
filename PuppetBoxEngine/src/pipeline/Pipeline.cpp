#include "puppetbox/Pipeline.h"

#include "../PipelineComponents.h"

namespace PB
{
    void Pipeline::init()
    {
        renderComponent_->init();

        //TODO: This is currently order dependent, revisit
        addComponent(std::make_unique<AIComponent>());
        addComponent(std::make_unique<ActionComponent>());
        addComponent(std::make_unique<PositionComponent>());
        addComponent(std::make_unique<PhysicsComponent>());
        addComponent(std::make_unique<AnimationComponent>());
    }

    void Pipeline::update(float deltaTime)
    {
        for (auto& objectComponent: objectComponents_)
        {
            objectComponent->update(deltaTime);
        }
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