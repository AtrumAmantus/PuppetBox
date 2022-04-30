#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include "DataStructures.h"
#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API IRenderComponent
    {
    public:
        virtual ~IRenderComponent() noexcept {};

        virtual void init() = 0;

        virtual void render() const = 0;

        virtual void addDataVector(std::string referenceName, std::shared_ptr<void> reference) = 0;

        virtual void setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap) = 0;

        virtual void sync(const std::function<void()>& process) = 0;

        virtual void lock() = 0;

        virtual void unlock() = 0;
    };
}