#pragma once

#include <memory>
#include <vector>

#include "AbstractObjectComponent.h"
#include "DataStructures.h"
#include "IRenderComponent.h"
#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API Pipeline
    {
    public:
        void init();

        void update(float deltaTime);

        void addComponent(std::unique_ptr<AbstractObjectComponent> component);

        void setRenderComponent(std::unique_ptr<IRenderComponent> renderComponent);

        void render() const;

        void tearDown();

    private:
        std::vector<std::unique_ptr<AbstractObjectComponent>> objectComponents_{};
        std::unique_ptr<IRenderComponent> renderComponent_{nullptr};
        std::vector<UUID> subscriptions_{};
    };
}