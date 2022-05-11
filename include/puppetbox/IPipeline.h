#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "AbstractObjectComponent.h"
#include "DataStructures.h"
#include "IRenderComponent.h"
#include "TypeDef.h"

namespace PB
{
    class PUPPET_BOX_API AbstractPipelineData
    {
    public:
        virtual ~AbstractPipelineData() noexcept;

        virtual const std::string getReference() const = 0;

        virtual std::shared_ptr<void> getDataVector() const = 0;

        virtual void init();

        void addDataSync(const UUID uuid);

        void removeDataAtSync(std::uint32_t index);

        void sync(const std::function<void()>& process);

        void lock();

        void unlock();

        void setEntityMap(std::unordered_map<UUID, std::uint32_t>* entityMap);

    protected:
        const std::unordered_map<UUID, std::uint32_t>& entityMap();

        virtual void addData(const UUID uuid) = 0;

        virtual void removeDataAt(std::uint32_t index) = 0;

    private:
        std::unordered_map<UUID, std::uint32_t>* entityMap_ = nullptr;
        std::mutex mutex_;
        bool isLocked_ = false;

    };

    class PUPPET_BOX_API IPipeline
    {
    public:
        virtual ~IPipeline() noexcept {};

        virtual void init() = 0;

        virtual void tearDown() = 0;

        virtual void update(float deltaTime) = 0;

        virtual void render() = 0;

        virtual void lockPipeline() = 0;

        virtual void unlockPipeline() = 0;

        virtual void addComponent(
                std::unique_ptr<AbstractObjectComponent> component,
                std::vector<std::string> vectorReferences) = 0;

        virtual void setRenderComponent(
                std::unique_ptr<IRenderComponent> renderComponent,
                const std::vector<std::string>& vectorReferences) = 0;

        virtual void addPipelineData(std::unique_ptr<AbstractPipelineData> pipelineData) = 0;
    };
}