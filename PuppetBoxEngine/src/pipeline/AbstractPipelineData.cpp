#include <exception>

#include "puppetbox/IPipeline.h"

namespace PB
{
    AbstractPipelineData::~AbstractPipelineData() noexcept
    {

    }

    void AbstractPipelineData::init()
    {

    }

    void AbstractPipelineData::addDataSync(const UUID uuid)
    {
        sync([this, uuid](){
            addData(uuid);
        });
    }

    void AbstractPipelineData::removeDataAtSync(std::uint32_t index)
    {
        sync([this, index]() {
            removeDataAt(index);
        });
    }

    void AbstractPipelineData::sync(const std::function<void()>& process)
    {
        std::unique_lock<std::mutex> mlock(mutex_);
        process();
    }

    void AbstractPipelineData::lock()
    {
        mutex_.lock();
        isLocked_ = true;
    }

    void AbstractPipelineData::unlock()
    {
        mutex_.unlock();
        isLocked_ = false;
    }

    void AbstractPipelineData::setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap)
    {
        entityMap_ = entityMap;
    }

    const std::unordered_map<UUID, std::uint32_t>& AbstractPipelineData::entityMap()
    {
        return *entityMap_;
    }
}