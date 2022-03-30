#include "puppetbox/AbstractObjectComponent.h"

namespace PB
{
    AbstractObjectComponent::AbstractObjectComponent() : mutex_(nullptr)
    {

    }

    AbstractObjectComponent::AbstractObjectComponent(std::mutex* mutex) : mutex_(mutex)
    {

    }

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

    }

    void AbstractObjectComponent::inits()
    {

    }

    std::unique_lock<std::mutex> AbstractObjectComponent::lock()
    {
        return std::unique_lock<std::mutex>(*mutex_);
    }
}