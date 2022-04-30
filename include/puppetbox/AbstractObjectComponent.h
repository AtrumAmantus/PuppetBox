#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "DataStructures.h"
#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API AbstractObjectComponent
    {
    public:
        virtual ~AbstractObjectComponent() noexcept;

        virtual void init();

        virtual void update(float deltaTime) = 0;

        virtual void addVectorReference(std::string referenceName, std::shared_ptr<void> reference);

        void setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap);

        void sync(const std::function<void()>& process);

        void lock();

        void unlock();

    protected:
        const std::unordered_map<UUID, std::uint32_t>& entityMap();

    private:
        std::unordered_map<UUID, std::uint32_t>* entityMap_ = nullptr;
        std::mutex mutex_;
        bool isLocked_ = false;

    };
}