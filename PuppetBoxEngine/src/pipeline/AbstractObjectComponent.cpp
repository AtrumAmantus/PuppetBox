#include "puppetbox/AbstractObjectComponent.h"

namespace PB
{
    AbstractObjectComponent::~AbstractObjectComponent() noexcept
    {

    }

    void AbstractObjectComponent::init()
    {

    }

    void AbstractObjectComponent::addVectorReference(std::string referenceName, std::shared_ptr<void> reference)
    {

    }

    void AbstractObjectComponent::setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap)
    {
        entityMap_ = entityMap;
    }

    void AbstractObjectComponent::sync(const std::function<void()>& process)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        process();
    }

    void AbstractObjectComponent::lock()
    {
        mutex_.lock();
        isLocked_ = true;
    }

    void AbstractObjectComponent::unlock()
    {
        mutex_.unlock();
        isLocked_ = false;
    }

    const std::unordered_map<UUID, std::uint32_t>& AbstractObjectComponent::entityMap()
    {
        return *entityMap_;
    }
}