#include "puppetbox/AbstractObjectComponent.h"

namespace PB
{
    void AbstractObjectComponent::init()
    {
        if (!isInitialized_)
        {
            isInitialized_ = true;

            inits();
        }
    }

    void AbstractObjectComponent::tearDown()
    {

    }

    void AbstractObjectComponent::update(float deltaTime)
    {
        // Lock this component while it runs its updates
        std::unique_lock<std::mutex> mlock{mutex_};

        updates(deltaTime);
    }

    void AbstractObjectComponent::inits()
    {

    }

    void AbstractObjectComponent::updates(float deltaTime)
    {

    }

    std::unique_lock<std::mutex> AbstractObjectComponent::createLock()
    {
        return std::unique_lock<std::mutex>(mutex_);
    }
}